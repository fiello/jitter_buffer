/**
 *  @file
 *  \brief     FrameFragment class declaration
 *  \details   Holds declaration of the FrameFragment class
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef VIDEO_CODING_FRAME_FRAGMENT_H
#define VIDEO_CODING_FRAME_FRAGMENT_H

// third-party
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>

namespace video_coding
{

class FrameFragment;
typedef boost::shared_ptr<FrameFragment> FrameFragmentPtr;

/**
 * FrameFragment class is a wrapper over a single fragment ever received by JitterBuffer
 * It holds buffer data, its length and fragment number. FrameBuffer uses this class as a
 * 'brick' to build the whole frame
 */
class FrameFragment
{
public:
   /**
    * Constructor
    * Copies buffer of the given size internally and also holds fragment number
    * for futher usage
    *
    * @param buffer - pointer to the input data
    * @param length - length of the buffer with input data
    * @param fragmentNumber - fragment number
    */
   FrameFragment(const char* buffer, int length, int fragmentNumber);

   /**
    * Accessor to get current fragment number
    * @returns - number of current fragment
    */
   int GetFragmentNumber() const;

   /**
    * Accessor to get current fragment data
    * @returns - pointer to the constant internal data buffer
    */
   const char* GetBufferData() const;

   /**
    * Accessor to get current fragment data length
    * @returns - data length
    */
   int GetBufferLength() const;

   /**
    * Overloaded operators, need it to help with object comparison
    */
   bool operator< (const FrameFragment& rhs);
   bool operator== (const int fragmentNumber);

private:
   /// fragment number
   const int                  m_fragmentNumber;
   /// length of buffer with data
   const int                  m_bufferLength;
   /// buffer with data (wrapped)
   boost::scoped_array<char>  m_bufferData;
};

} // namespace video_coding

#endif // VIDEO_CODING_FRAME_FRAGMENT_H
