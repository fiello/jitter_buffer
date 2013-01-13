#ifndef VIDEO_CODING_TEST_STUB_DECODER_H
#define VIDEO_CODING_TEST_STUB_DECODER_H

#include <video_engine/interface/decoder.h>
#include <memory.h>

namespace video_coding
{
namespace test
{

const int DecoderMaxOutputSize = 1024 * 1024;   // 1Mb

class StubDecoder : public ::video_engine::IDecoder
{
public:
   virtual int DecodeFrame(const char* buffer, int length, char* outputBuffer)
   {
      int outputSize = (length < DecoderMaxOutputSize) ? length : DecoderMaxOutputSize;
      ::memcpy(outputBuffer, buffer, outputSize);
      return length;
   }
};


} // namespace test
} // namespace video_coding

#endif // VIDEO_CODING_TEST_STUB_DECODER_H
