//
// Created by pspr1 on 4/16/2021.
//

#include "TextView.h"
TextView::TextView()
{
    glGenBuffers(1,&vertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
TextView::~TextView()
{
    if(glIsBuffer(vertexBufId))
        glDeleteBuffers(1,&vertexBufId);
}

void TextView::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    float *vertices=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*8,GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    if(vertices)
    {
        ///X's
        vertices[0] = -1.0 + (startX * 2.0) /(float) displayParams.screenWidth;//*2 so that-1 to 1 else we get 0 to 1 after conversion  leftX
        vertices[2] = -1.0 + ((startX + width) * 2.0) / (float) displayParams.screenWidth;//rightX
        vertices[4] = vertices[2];
        vertices[6] = vertices[0];
        ///Y's
        vertices[1] = 1.0 - ((startY + height) * 2.0) / (float) displayParams.screenHeight;//bottomy
        vertices[3] = vertices[1];
        vertices[5] = 1.0 - ((startY) * 2.0) / (float) displayParams.screenHeight;//topy
        vertices[7] = vertices[5];
    }
    else
    {////lazy draw on
        ///uploading vertices everydrawcall
        UILogE("ImageView::setBouds-error uploading vertices");
        Graphics::printGlError("ImageView::setBouds()");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    glBindBuffer(GL_ARRAY_BUFFER,0);

}
void TextView::draw()
{
    clearRect();
}