/**
 *  @file
 *  \brief     FrameBuffer class declaration
 *  \details   Holds declaration of the FrameBuffer class
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef VIDEO_CODING_FRAME_BUFFER_H
#define VIDEO_CODING_FRAME_BUFFER_H

#include "frame_fragment.h"
#include <common/result_code.h>
// third-party
#include <boost/shared_ptr.hpp>
#include <vector>

namespace video_coding
{

class FrameBuffer;
typedef boost::shared_ptr<FrameBuffer> FrameBufferPtr;

/**
 * FrameBuffer class represents a holder which maintains a list of fragments
 * (frame_fragment.cc) in scope of one particular frame and a bunch of operations
 * upon them.
 */
class FrameBuffer
{
public:

   /**
    * Constructor
    * Invokes AppendFragment directly
    * @param buffer - pointer to the input data
    * @param length - length of the buffer with input data
    * @param frameNumber - frame number that new fragment of data belongs to
    * @param fragmentNumber - fragment number
    * @param numFragmentsInThisFrame - number of fragments we expect to receive to mark this
    *                                  frame as completed
    */
   FrameBuffer(const char* buffer,
      int length,
      int frameNumber,
      int fragmentNumber,
      int numFragmentsInThisFrame);

   /**
    * Method to append new fragment to the frame. Manages
    *  - frame completion flag
    *  - reject of retransmitted fragments
    *  - creation of new FrameFragment
    *
    * @param buffer - pointer to the input data
    * @param length - length of the buffer with input data
    * @param fragmentNumber - fragment number
    */
   void AppendFragment(const char* buffer, int length, int fragmentNumber);

   /**
    * Assembles the whole frame from the array of fragments
    * @param outputBuffer - in/out parameter, will contain assembled frame data. Size of
    *                       complete frame can be retrieved by GetCurrentFrameSize
    */
   void GetAssembledData(char* outputBuffer);

   /**
    * Accessor to get current frame number
    * @returns - number of current frame
    */
   int GetFrameNumber() const;

   /**
    * Accessor to get current frame size in bytes summarizing all fragments
    * @returns - size of current frame (in bytes)
    */
   int GetCurrentFrameSize() const;

   /**
    * Accessor to get flag of frame completion
    * @returns - true if frame is completed and all fragments are in place,
    *            false otherwise
    */
   bool IsFrameComplete() const;

private:
   typedef std::vector<FrameFragmentPtr> FrameFragments;

   /// frame number
   const int         m_frameNumber;
   /// number of fragments exepcted in this frame
   const int         m_numFragmentsInThisFrame;
   /// flag, indicates if frame is complete and can be assembled/decoded
   bool              m_frameIsComplete;
   /// holds current frame size (in bytes) - summary of all fragments sizes
   int               m_currentFrameSize;
   /// container that holds frame fragments
   FrameFragments    m_frameFragments;
};

} // namespace video_coding

#endif // VIDEO_CODING_FRAME_BUFFER_H
