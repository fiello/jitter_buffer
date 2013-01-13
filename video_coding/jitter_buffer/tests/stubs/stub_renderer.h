#ifndef VIDEO_CODING_TEST_STUB_RENDERER_H
#define VIDEO_CODING_TEST_STUB_RENDERER_H

#include <video_engine/interface/renderer.h>
// third-party
#include <boost/thread/barrier.hpp>

namespace video_coding
{
namespace test
{

class StubRenderer : public ::video_engine::IRenderer
{
public:
   StubRenderer()
      : m_dataLength(0)
   {}

   void RenderFrame(const char* buffer, int length)
   {
      std::string tempString;
      tempString.assign(buffer, length);
      m_renderData += tempString;
      m_dataLength += length;
      return;
   }

   std::string GetRenderedData()
   {
      return m_renderData;
   }

private:
   std::string m_renderData;
   int         m_dataLength;
};

} // namespace test
} // namespace video_coding

#endif // VIDEO_CODING_TEST_STUB_RENDERER_H
