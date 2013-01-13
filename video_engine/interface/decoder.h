/**
 *  @file
 *  \brief     video_engine::IDecoder interface
 *  \details   Declares IDecoder interface
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef VIDEO_ENGINE_DECODER_H
#define VIDEO_ENGINE_DECODER_H

namespace video_engine
{

class IDecoder
{
public:

   /**
    * Returns the size of the data written to the outputBuffer, will be no more than 1mb.
    * @param buffer - input frame data (raw data without any headers)
    * @param length - length of the data to be passed
    * @param outputBuffer - pointer to the output data
    * @returns size of the decoded data
    */
   virtual int DecodeFrame(const char* buffer, int length, char* outputBuffer) = 0;

   ~IDecoder() {}
};


} // namespace video_engine

#endif // VIDEO_ENGINE_DECODER_H
