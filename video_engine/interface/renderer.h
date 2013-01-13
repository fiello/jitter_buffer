/**
 *  @file
 *  \brief     video_engine::IRenderer interface
 *  \details   Declares IRenderer interface
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#ifndef VIDEO_ENGINE_RENDERER_H
#define VIDEO_ENGINE_RENDERER_H

namespace video_engine
{

class IRenderer
{
public:

   /**
    * Renders the given buffer. This call will not block for any significant period, and
    * the buffer will be copied internally so can be deleted/reused as soon as this call
    * is completed.
    * @param buffer - pointer to the data to be rendered
    * @param length - length of the buffer
   */
   virtual void RenderFrame(const char* buffer, int length) = 0;

   ~IRenderer() {}
};


} // namespace video_engine

#endif // VIDEO_ENGINE_RENDERER_H
