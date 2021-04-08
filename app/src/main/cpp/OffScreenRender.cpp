//
// Created by Shiva Shankar patel psy on 2/12/21.
//
#include "OffScreenRender.h"
#include "Graphics.h"
//setup texture,render buffer,gen framebuffer configure;

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1,&id);
  //  fbImage.setTextureId(textureToRender);
}
FrameBuffer::FrameBuffer(int width, int height):FrameBuffer()
{
    setDims(width,height);
    configureColorBuffer();
    configureDepthBuffer();////optional remove if not needed
    configure();
}
FrameBuffer::FrameBuffer(Bitmap *image):FrameBuffer( image->width,image->height)
{

}
void FrameBuffer::setDims(int width, int height)
{
    this->width=width;
    this->height=height;
}
void FrameBuffer::configureColorBuffer()
{

    glGenTextures(1,&texId);
    glBindTexture(GL_TEXTURE_2D,texId);
  //  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,0);//just make GL_RGB for color only.
    glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,width,height);///changed from above to this cuz error in bindimaageTexture;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//more filteres
    glBindTexture(GL_TEXTURE_2D,0);
    Graphics::printGlError("configure FrameBuffer ColorBuffer");
}
void FrameBuffer::configureDepthBuffer()
{
    //optional DepthBuffer if depth testing needed
    glBindFramebuffer(GL_FRAMEBUFFER,id);
    glGenRenderbuffers(1,&depthBufId);
    glBindRenderbuffer(GL_RENDERBUFFER,depthBufId);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_COMPONENT,GL_RENDERBUFFER,depthBufId);
    setToDefault();
}

void FrameBuffer::configure()
{
    //configure the frame buffer.
    glBindFramebuffer(GL_FRAMEBUFFER,id);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texId,0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // draw to the specified  buffers attached to this frame buffer listed in above line.

    //draw to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,id);

    // Always check framebuffe status is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        //error dfsdf
        Loge("frameBufferStatus","error");
        //

    }
    setToDefault();

}
void FrameBuffer::makeActive()
{
    glBindFramebuffer(GL_FRAMEBUFFER,id);
    glViewport(0,0,width,height);///change immediate after fbo rendering done to default screen
  //  Loge("FrameBuffer::makeActive","width %d and height %d",width,height);
}
void FrameBuffer::setToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight);

}