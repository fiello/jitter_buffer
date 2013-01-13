/**
 *  @file
 *  \brief     FrameBuffer class implementation
 *  \details   Holds implementation of the FrameBuffer class
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#include "frame_buffer.h"
#include <logger/logger.h>
// third-party
#include <string.h>
#include <boost/bind.hpp>

namespace
{

/**
 * Helper function to perform comparison between two FrameFragment pointers
 * @param lhs - left-hand side frame fragment
 * @param rhs - right-hand side frame fragment
 * @returns - true if lhs less than rhs, false otherwise
 */

static bool FragmentCompareLess(
   const video_coding::FrameFragmentPtr& lhs,
   const video_coding::FrameFragmentPtr& rhs)
{
   return *(lhs.get()) < *(rhs.get());
}

/**
 * Helper function to perform comparison between two fragmentNumber and FrameFragment object
 * @param fragmentNumber - left-hand side frame fragment
 * @param rhs - right-hand side frame fragment
 * @returns - true if FrameFragment has  the same fragment number
 */
static bool FragmentCompareEqual(const int fragmentNumber, const video_coding::FrameFragmentPtr& rhs)
{
   return *(rhs.get()) == fragmentNumber;
}

} // unnamed namespace

namespace video_coding
{

FrameBuffer::FrameBuffer(const char* buffer,
         const int length,
         const int frameNumber,
         const int fragmentNumber,
         const int numFragmentsInThisFrame)
   : m_frameNumber(frameNumber)
   , m_numFragmentsInThisFrame(numFragmentsInThisFrame)
   , m_frameIsComplete(false)
   , m_currentFrameSize(0)
{
   AppendFragment(buffer, length, fragmentNumber);
}

void FrameBuffer::AppendFragment(const char* buffer, int length, int fragmentNumber)
{
   if (m_frameIsComplete)
      return;

   FrameFragments::const_iterator it;

   int count = std::count_if(
         m_frameFragments.begin(),
         m_frameFragments.end(),
         boost::bind(FragmentCompareEqual, fragmentNumber, _1) );

   if (count)
   {
      LOGDBG << "Retransmitted fragment #" << fragmentNumber;
      return;
   }

   m_currentFrameSize += length;
   FrameFragmentPtr newFragment( new FrameFragment(buffer, length, fragmentNumber) );
   m_frameFragments.push_back(newFragment);

   if (m_frameFragments.size() == (size_t)m_numFragmentsInThisFrame)
      m_frameIsComplete = true;
}

void FrameBuffer::GetAssembledData(char* outputBuffer)
{
   std::sort(m_frameFragments.begin(), m_frameFragments.end(), FragmentCompareLess);

   int currentPos = 0;
   int bufferLength = 0;
   for (size_t i = 0; i < m_frameFragments.size(); ++i)
   {
      bufferLength = m_frameFragments[i]->GetBufferLength();
      ::memcpy(outputBuffer + currentPos,
            m_frameFragments[i]->GetBufferData(),
            bufferLength);
      currentPos += bufferLength;
   }
}

int FrameBuffer::GetFrameNumber() const
{
   return m_frameNumber;
}

int FrameBuffer::GetCurrentFrameSize() const
{
   return m_currentFrameSize;
}

bool FrameBuffer::IsFrameComplete() const
{
   return m_frameIsComplete;
}

} // namespace video_coding

