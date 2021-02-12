//
// Created by Shiva Shankar patel psy on 2/12/21.
//

#ifndef PHOTOFX_OFFSCREENRENDER_H
#define PHOTOFX_OFFSCREENRENDER_H
#include "Graphics.h"
class FrameBuffer{
private:
    GLuint id=0;
    GLuint texId=0;//color attachment check internal and external formats later.
    GLuint depthBufId=0;//optional DepthBuffer if depth testing needed
public:
    FrameBuffer();//by default creates screenSized buffer for other size use other Construtors.
    GLuint getId(){return id;}//combine to make return all ids using one function.
    GLuint getTexId(){return texId;}
    static void setToDefault();
    void makeActive();



};
#endif //PHOTOFX_OFFSCREENRENDER_H
