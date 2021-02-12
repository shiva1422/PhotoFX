//
// Created by Shiva Shankar patel psy on 2/12/21.
//
#include "OffScreenRender.h"
#include "Graphics.h"
void FrameBuffer::makeActive()
{
    glBindFramebuffer(GL_FRAMEBUFFER,id);
}
void FrameBuffer::setToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}
FrameBuffer::FrameBuffer()
{
 //setup texture,render buffer,gen framebuffer configure;
    glGenFramebuffers(1,&id);
    glBindFramebuffer(GL_FRAMEBUFFER,id);

    glGenTextures(1,&texId);
    glBindTexture(GL_TEXTURE_2D,texId);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,0);//just make GL_RGB for color only.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //optional DepthBuffer if depth testing needed
    glGenRenderbuffers(1,&depthBufId);
    glBindRenderbuffer(GL_RENDERBUFFER,depthBufId);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_COMPONENT,GL_RENDERBUFFER,depthBufId);

    //configure the frame buffer.
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texId,0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers this buffer draws to the specified bufferes.

    //draw to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER,id);
    glViewport(0,0,Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight);


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        //error dfsdf
        Loge("frameBufferStatus","error");
        //

    }


//
  //  fbImage.setTextureId(textureToRender);






}