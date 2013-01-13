/**
 *  @file
 *  \brief     JitterBuffer factory implementation
 *  \details   Holds implementation of factory method that can be used to create
 *             instance of JitterBuffer
 *  \author    Dmitry Sinelnikov
 *  \date      2012
 */

#include <video_coding/interface/jitter_buffer.h>
#include "jitter_buffer_impl.h"

namespace video_coding
{

boost::shared_ptr<IJitterBuffer> CreateJitterBuffer(IDecoder* decoder, IRenderer* renderer)
{
   boost::shared_ptr<IJitterBuffer> jitterBuffer;
   jitterBuffer.reset( new JitterBufferImpl(decoder, renderer) );
   return jitterBuffer;
}

IJitterBuffer::IJitterBuffer(IDecoder* decoder, IRenderer* renderer)
{}

} // namespace video_coding
