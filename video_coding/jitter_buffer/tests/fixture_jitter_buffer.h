#ifndef VIDEO_CODING_TEST_FIXTURE_JITTER_BUFFER_H
#define VIDEO_CODING_TEST_FIXTURE_JITTER_BUFFER_H

#include "stubs/stub_decoder.h"
#include "stubs/stub_renderer.h"
#include <video_coding/interface/jitter_buffer.h>
#include <common/result_code.h>
#include <logger/logger.h>
// third-party
#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>

namespace video_coding
{
namespace test
{

typedef boost::shared_ptr<IJitterBuffer> JitterBufferPtr;
typedef boost::shared_ptr<StubDecoder> StubDecoderPtr;
typedef boost::shared_ptr<StubRenderer> StubRendererPtr;

class FixtureJitterBuffer : public ::testing::Test
{
public:
   FixtureJitterBuffer();
   void SetUp();
   void TearDown();

   result_t CreateJB(IDecoder* decoder, IRenderer* renderer);
   result_t CheckReceiverFunction(const char* buffer,
         const int length,
         const int frameNumber,
         const int fragmentNumber,
         const int numFragmentsInThisFrame);

   JitterBufferPtr GetJB();
   StubDecoderPtr GetDecoder();
   StubRendererPtr GetRenderer();

private:
   /// Instance of the real IJItterBuffer implementation we need to test
   JitterBufferPtr   m_jitterBuffer;
   /// Stub for decoder component
   StubDecoderPtr    m_decoder;
   /// Stub for renderer component
   StubRendererPtr   m_renderer;
   /// Loglevel we need to save before test execution. Will be restored in a
   /// TearDown procedure
   logger::LevelId   m_savedLogLevel;
};

} // namespace test
} // namespace video_coding

#endif // VIDEO_CODING_TEST_FIXTURE_JITTER_BUFFER_H
