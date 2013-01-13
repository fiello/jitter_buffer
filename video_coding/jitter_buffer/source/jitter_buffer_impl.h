/**
 *  @file
 *  \brief     JitterBufferImpl class declaration
 *  \details   Holds declaration of the main class JitterBufferImpl
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef VIDEO_CODING_JITTER_BUFFER_IMPL_H
#define VIDEO_CODING_JITTER_BUFFER_IMPL_H

#include <video_coding/interface/jitter_buffer.h>
#include "frame_buffer.h"
// third-party
#include <map>
#include <list>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace video_coding
{

/**
 * JitterBufferImpl class
 * Implements interface IJitterBuffer.
 */
class JitterBufferImpl
   : public IJitterBuffer
   , boost::noncopyable
{
public:

   /**
    * Constructor. For more details about behavior and input arguments take a look
    * at the IJItterBuffer interface declaration
    */
   JitterBufferImpl(IDecoder* decoder, IRenderer* renderer);

   /**
    * Destructor. Performs component tear down procedure, stops running threads
    */
   virtual ~JitterBufferImpl();

   /**
    * IJitterBuffer interface method implementation. Responsible for receiving and storing
    * incoming packets in internal buffer. For more details see IJitterBuffer interface.
    */
   virtual void ReceivePacket(
      const char* buffer,
      int length,
      int frameNumber,
      int fragmentNumber,
      int numFragmentsInThisFrame);

private:
   typedef boost::lock_guard<boost::mutex> LOCK;
   typedef std::map<int, FrameBufferPtr> FrameBuffers;
   typedef std::list<FrameBufferPtr> FrameList;

   /**
    * Recycler thread main routine. Thread is running in a loop in this function
    * unless shutdown is requested. Upon the shutdown unprocessed fragments will be
    * purged without processing.
    * The main aim of this function is to traverse through the list of frames and
    * identify if any frame is ready for decoding. Ready-to-decode frames must satisfy
    * two requirements:
    *  - all fragments were received;
    *  - next frame in a sequence to be decoded equals this frame number
    */
   void RecycleExistingFrames();

   /**
    * Decoder thread main routine. Thread is running in a loop in this function
    * unless shutdown is requested. Upon the shutdown unprocessed fragments will be
    * purged without processing.
    * According to Decoder contract, frames can be processed only when there are no
    * gaps. If known exception occurs during frame processing (decoding or rendering)
    * then component triggers a error and stops receiving new packets to notify the caller
    * about the issue.
    */
   void ProcessCompletedFrames();

   /// raw pointer to the instance which implements IDecoder interface
   IDecoder*                              m_decoder;
   /// raw pointer to the instance which implements IRenderer interface
   IRenderer*                             m_renderer;

   /// mutex to grant exclusive access to the buffer with unsorted frames
   boost::mutex                           m_unsortedFrameBuffersGuard;
   /// container which holds pair [frameNumber, FrameBuffer]. Stores buffers with
   /// all fragments of incoming frames (except for empty one and retransmitted),
   /// in the order of arrival
   FrameBuffers                           m_unsortedFrameBuffers;

   /// mutex to grant exclusive access to the buffer with sorted frames
   /// that are ready for decoding
   boost::mutex                           m_sortedFrameBuffersGuard;
   /// container which holds only completed frames - frames which have all
   /// fragments received. Frames are placed in this container in the
   /// proper order (sorted, ready for decoding)
   FrameList                              m_sortedFrameBuffers;
   /// Indicates last decoded frame number
   int                                    m_lastDecodedFrameNumber;

   /// Condition variable to notify recycle task about new incoming fragments
   boost::condition_variable              m_recycleCondition;
   /// Indicates if recycle task has already been launched
   bool                                   m_recycleTaskLaunched;
   /// Condition variable to notify decoder task that new frame is ready
   /// for decoding
   boost::condition_variable              m_decoderCondition;
   /// Indicates if decoding task has already been launched
   bool                                   m_dataProcessingTaskLaunched;

   /// Recycle thread will be used to traverse through the list of
   /// available unsorted frames and move them to sorted buffer.
   /// Will be launched with the first incoming data fragment
   /// (delayed initialization)
   boost::scoped_ptr<boost::thread>       m_recyclerThread;

   /// Decoder thread will be used to traverse through the list of
   /// sorted frames, adjust fragments and pass this frame to decoder
   /// and then to the renderer. Will be launched with the first incoming
   /// data fragment (delayed initialization)
   boost::scoped_ptr<boost::thread>       m_dataProcessingThread;

   /// Flag that component shutdown has been requested
   bool                                   m_shutdownRequested;

   /// flag indicates some critical error at video processing stage
   bool                                   m_frameProcessingIsBlocked;
};

} // namespace video_coding

#endif // VIDEO_CODING_JITTER_BUFFER_IMPL_H
