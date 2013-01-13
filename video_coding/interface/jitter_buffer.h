/**
 *  @file
 *  \brief     video_coding::IJitterBuffer interface
 *  \details   Holds declaration of the IJitterBuffer interface and defines interface contract
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef VIDEO_CODING_JITTER_BUFFER_H
#define VIDEO_CODING_JITTER_BUFFER_H

#include <boost/shared_ptr.hpp>

namespace video_engine
{
   class IDecoder;
   class IRenderer;
}

namespace video_coding
{

using video_engine::IDecoder;
using video_engine::IRenderer;

class IJitterBuffer;

/**
 * Single factory function which creates instance of the IJitterBuffer
 * component. Caller must be prepared to handle std::exception thrown
 * in case of invalid input arguments
 *
 * @param decoder - raw pointer to the decoder object
 * @param renderer - raw pointer to the renderer object
 * @returns - shared_ptr holding pointer to the newly created instance of
 *            JitterBuffer component
 */
boost::shared_ptr<IJitterBuffer> CreateJitterBuffer(
        IDecoder* decoder,
        IRenderer* renderer);

/**
 * IJitterBuffer component external interface.
 */
class IJitterBuffer
{
public:

   /**
    * Constructor.
    * Caller must be prepared to handle std::exception with InvalidArgument
    * result code in case of invalid input parameters. Both input arguments are stored
    * inside JB as raw pointers and not release on object destruction. Therefore
    * lifespan of both Decoder and Renderer must be handled by external caller.
    *
    * @param decoder - raw pointer to the decoder. Will be used to decode incoming frames.
    * @param renderer - raw pointer to the renderer. Will be used to render decoded frames
    */
   IJitterBuffer(IDecoder* decoder, IRenderer* renderer);

   /**
    * Should copy the given buffer, as it may be deleted/reused immediately following
    * this call. This method will not block the call for significant period of time.
    * Decoding and Rendering will be performed by internal threads running separately.
    * Caller must be prepared to handle std::exception thrown from this function in case
    * of invalid input arguments or internal error (buffer overflow, etc)
    *
    * @param buffer - incoming data buffer
    * @param length - size of the incoming data buffer
    * @param frameNumber - will start at zero for the call
    * @param fragmentNumber - specifies what position this fragment is within the given
    *                         frame - the first fragment number in each frame is number zero
    * @param numFragmentsInThisFrame - is guaranteed to be identical for all fragments
    *                                  with the same frameNumber
    */
   virtual void ReceivePacket(
      const char* buffer,
      int length,
      int frameNumber,
      int fragmentNumber,
      int numFragmentsInThisFrame) = 0;

   ~IJitterBuffer() {}
};

} // namespace video_coding

#endif // VIDEO_CODING_JITTER_BUFFER_H
