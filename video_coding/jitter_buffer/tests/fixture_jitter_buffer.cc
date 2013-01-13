
#include "fixture_jitter_buffer.h"
#include <common/exception_dispatcher.h>
// third-party
#include <boost/thread.hpp>

namespace video_coding
{
namespace test
{

FixtureJitterBuffer::FixtureJitterBuffer()
{}

void FixtureJitterBuffer::SetUp()
{
   m_decoder.reset( new StubDecoder() );
   m_renderer.reset( new StubRenderer() );

   // set most detailed log level
   using namespace logger;
   m_savedLogLevel = Log::GetLogLevel();
   Log::SetLogLevel(Warning);
}

void FixtureJitterBuffer::TearDown()
{
   m_jitterBuffer.reset();
   m_renderer.reset();
   m_decoder.reset();
   logger::Log::SetLogLevel(m_savedLogLevel);
}

result_t FixtureJitterBuffer::CreateJB(IDecoder* decoder, IRenderer* renderer)
{
   try
   {
      m_jitterBuffer = CreateJitterBuffer(decoder, renderer);
      return result_code::sOk;
   }
   catch(const std::exception&)
   {
      return exception::ExceptionDispatcher::Dispatch(BOOST_CURRENT_FUNCTION);
   }
}

result_t FixtureJitterBuffer::CheckReceiverFunction(
      const char* buffer,
      const int length,
      const int frameNumber,
      const int fragmentNumber,
      const int numFragmentsInThisFrame)
{
   try
   {
      JitterBufferPtr jitterBuffer = GetJB();
      jitterBuffer->ReceivePacket(buffer, length, frameNumber, fragmentNumber, numFragmentsInThisFrame);
      return result_code::sOk;
   }
   catch(const std::exception&)
   {
      return exception::ExceptionDispatcher::Dispatch(BOOST_CURRENT_FUNCTION);
   }
}


boost::shared_ptr<IJitterBuffer> FixtureJitterBuffer::GetJB()
{
   if (!m_jitterBuffer)
      CreateJB(m_decoder.get(), m_renderer.get());

   return m_jitterBuffer;
}

boost::shared_ptr<StubDecoder> FixtureJitterBuffer::GetDecoder()
{
   return m_decoder;
}

boost::shared_ptr<StubRenderer> FixtureJitterBuffer::GetRenderer()
{
   return m_renderer;
}

} // namespace test
} // namespace video_engine
