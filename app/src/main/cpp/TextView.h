//
// Created by pspr1 on 4/16/2021.
//

#ifndef PHOTOFX_TEXTVIEW_H
#define PHOTOFX_TEXTVIEW_H


#include "UI.h"

class TextView : public View {
protected:
    Bitmap textImage;
    GLuint texId=0,texBufId=0,vertexBufId=0;
public:
    TextView();
    ~TextView();
    virtual void setBounds(float startX,float startY,float width,float height) override ;
    virtual void draw() override ;
};


#endif //PHOTOFX_TEXTVIEW_H
