/**
 *  @file
 *  \brief     JitterBufferImpl class implementation
 *  \details   Holds implementation of JitterBufferImpl class
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#include "jitter_buffer_impl.h"
#include "frame_buffer.h"
#include <common/result_code.h>
#include <common/exception_dispatcher.h>
#include <video_engine/interface/decoder.h>
#include <video_engine/interface/renderer.h>

namespace video_coding
{

/// maximum number of unprocessed (incomplete) frames that can be
/// stored inside a JitterBuffer
static const int MaxFrameNumber = 100;

/// maximum data size that can be returned by the decoder after
/// the frame is processed
static const int MaxDecodedBufferSize = 1024 * 1024; // 1Mb

JitterBufferImpl::JitterBufferImpl(IDecoder* decoder, IRenderer* renderer)
   : IJitterBuffer(decoder, renderer)
   , m_lastDecodedFrameNumber(-1)
   , m_recycleTaskLaunched(false)
   , m_dataProcessingTaskLaunched(false)
   , m_shutdownRequested(false)
   , m_frameProcessingIsBlocked(false)
{
   CHECK_ARGUMENT(decoder != 0, "Decoder is zero!");
   CHECK_ARGUMENT(renderer != 0, "Renderer is zero!");
   m_decoder = decoder;
   m_renderer = renderer;
}

JitterBufferImpl::~JitterBufferImpl()
{
   m_shutdownRequested = true;

   if (m_recyclerThread.get())
      m_recyclerThread->join();

   if (m_dataProcessingThread.get())
      m_dataProcessingThread->join();
}

void JitterBufferImpl::ReceivePacket(
   const char* buffer,
   const int length,
   const int frameNumber,
   const int fragmentNumber,
   const int numFragmentsInThisFrame)
{
   try
   {
      CHECK_ARGUMENT(buffer != 0, "Buffer data is zero!");
      CHECK_ARGUMENT(length > 0, "Buffer data is empty!");
      CHECK_ARGUMENT(frameNumber >= 0, "Frame number must be non-negative!");
      CHECK_ARGUMENT(fragmentNumber >= 0, "Fragment number must be non-negative!");
      CHECK_ARGUMENT(numFragmentsInThisFrame > 0, "Frame must have at least 1 fragment!");

      // let the caller know that JB is broken (either of worker threads encountered
      // critical error and therefore component is unable to function properly further)
      if (m_frameProcessingIsBlocked)
         THROW_BASIC_EXCEPTION(result_code::eFail) << "Frame processing is blocked!";

      // start new section here to limit the scope where locker is used
      {  // Prefer this local scope as new function would require list of input params
         // compared to what we have in 'ReceivePacket' method
         LOCK lock(m_unsortedFrameBuffersGuard);
         if (frameNumber <= m_lastDecodedFrameNumber)
         {
            LOGDBG << "Frame #" << frameNumber<< " is already processed, skip it";
            return;
         }

         if (m_unsortedFrameBuffers.size() == (size_t)MaxFrameNumber)
            THROW_BASIC_EXCEPTION(result_code::eOutOfSpace) << "Jitter Buffer is full";

         FrameBuffers::const_iterator it = m_unsortedFrameBuffers.find(frameNumber);
         if (it == m_unsortedFrameBuffers.end())
         {
            LOGDBG << "New frame #" << frameNumber << " arrived (fragment #"
                   << fragmentNumber << " of " << numFragmentsInThisFrame << ")";

            FrameBufferPtr frameBuffer( new FrameBuffer(
                  buffer,
                  length,
                  frameNumber,
                  fragmentNumber,
                  numFragmentsInThisFrame) );

            m_unsortedFrameBuffers[frameNumber] = frameBuffer;
         }
         else
         {
            // fragment of some old frame
            LOGDBG << "Frame #" << frameNumber << " got new fragment #" << fragmentNumber;
            it->second->AppendFragment(buffer, length, fragmentNumber);
         }
      }

      if (!m_recycleTaskLaunched)
      {
         m_recyclerThread.reset( new boost::thread(
               boost::bind(&JitterBufferImpl::RecycleExistingFrames, this)) );
         m_recycleTaskLaunched = true;
      }

      if (!m_dataProcessingTaskLaunched)
      {
         m_dataProcessingThread.reset( new boost::thread(
               boost::bind(&JitterBufferImpl::ProcessCompletedFrames, this)) );
         m_dataProcessingTaskLaunched = true;
      }

      // notify recycler thread every time the new fragment arrives - this will help
      // keeping frame buffer free from old completed frames
      m_recycleCondition.notify_one();
   }
   catch(const std::exception&)
   {
      // Let dispatcher trace exception source: can be helpful in revising call stack in case
      // of exceptions from underlying components
      exception::ExceptionDispatcher::Dispatch(BOOST_CURRENT_FUNCTION);
      throw;
   }
}

void JitterBufferImpl::RecycleExistingFrames()
{
   try
   {
      FrameBufferPtr frameBuffer;
      int frameNumber;
      FrameList tempArray;
      FrameBuffers::const_iterator it;

      while (!m_shutdownRequested)
      {

         { // loop through unsorted frames
            boost::unique_lock<boost::mutex> lock(m_unsortedFrameBuffersGuard);
            m_recycleCondition.timed_wait(lock, boost::posix_time::milliseconds(5));

            it = m_unsortedFrameBuffers.begin();
            while (it != m_unsortedFrameBuffers.end())
            {
               frameBuffer = it->second;
               frameNumber = it->first;
               ++it;
               if ( frameBuffer->IsFrameComplete() &&
                   (frameNumber == (m_lastDecodedFrameNumber + 1)))
               {
                  tempArray.push_back(frameBuffer);
                  m_unsortedFrameBuffers.erase(frameNumber);
                  ++m_lastDecodedFrameNumber;
               }
            }
         }

         // if old completed frames were found and they have appropriate
         if (!tempArray.empty())
         {
            LOCK lock(m_sortedFrameBuffersGuard);
            m_sortedFrameBuffers.insert(
                  m_sortedFrameBuffers.end(),
                  tempArray.begin(),
                  tempArray.end());
            m_decoderCondition.notify_one();
         }

         tempArray.clear();
         frameBuffer.reset();
      } // while (!m_shutdownRequested)
   }
   catch (const std::exception&)
   {
      // log error but do not throw as it's a thread routine
      exception::ExceptionDispatcher::Dispatch(BOOST_CURRENT_FUNCTION);
      m_frameProcessingIsBlocked = true;
   }
}

void JitterBufferImpl::ProcessCompletedFrames()
{
   try
   {
      FrameBufferPtr frameBuffer;

      // since Decoder response size is fixed we can allocate buffer once
      boost::scoped_array<char> decodedData( new char[MaxDecodedBufferSize] );

      while (!m_shutdownRequested)
      {
         { // locker scope
            boost::unique_lock<boost::mutex> lock(m_sortedFrameBuffersGuard);
            m_decoderCondition.timed_wait(lock, boost::posix_time::milliseconds(5));

            if (m_sortedFrameBuffers.empty())
               continue;

            frameBuffer = m_sortedFrameBuffers.front();
            m_sortedFrameBuffers.pop_front();
         }

         LOGDBG << "Reassembling frame #" << frameBuffer->GetFrameNumber();
         int currentFrameSize = frameBuffer->GetCurrentFrameSize();

         boost::scoped_array<char> frameData( new char[currentFrameSize] );
         ::memset(frameData.get(), 0, currentFrameSize);
         frameBuffer->GetAssembledData(frameData.get());

         int decodedBufferSize = m_decoder->DecodeFrame(frameData.get(),
                              currentFrameSize,
                              decodedData.get());

         m_renderer->RenderFrame(decodedData.get(), decodedBufferSize);

      } // while (!m_shutdownRequested)
   }
   catch (const std::exception&)
   {
      // log error but do not throw as it's a thread routine
      exception::ExceptionDispatcher::Dispatch(BOOST_CURRENT_FUNCTION);
      m_frameProcessingIsBlocked = true;
   }
}


} // namespace video_coding
