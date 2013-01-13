/**
 *  @file
 *  \brief     FrameFragment class implementation
 *  \details   Holds implementation of the FrameFragment class
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#include "frame_fragment.h"
#include <common/exception_dispatcher.h>
// third-party
#include <string.h>

namespace video_coding
{

FrameFragment::FrameFragment(const char* buffer, const int length, const int fragmentNumber)
   : m_fragmentNumber(fragmentNumber)
   , m_bufferLength(length)
{
   try
   {
      m_bufferData.reset( new char[length] );
      ::memcpy(m_bufferData.get(), buffer, length);
   }
   catch(std::exception& ex)
   {
      // let exception propagate further to signal that the object is not created
      exception::ExceptionDispatcher::Dispatch(BOOST_CURRENT_FUNCTION);
      throw;
   }
}

int FrameFragment::GetFragmentNumber() const
{
   return m_fragmentNumber;
}

const char* FrameFragment::GetBufferData() const
{
   return m_bufferData.get();
}

int FrameFragment::GetBufferLength() const
{
   return m_bufferLength;
}

bool FrameFragment::operator< (const FrameFragment& rhs)
{
   return m_fragmentNumber < rhs.m_fragmentNumber;
}

bool FrameFragment::operator== (const int fragmentNumber)
{
   return m_fragmentNumber == fragmentNumber;
}

} // namespace video_coding
