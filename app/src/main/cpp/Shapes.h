//
// Created by Shiva Shankar patel psy on 4/10/21.
//

#ifndef PHOTOFX_SHAPES_H
#define PHOTOFX_SHAPES_H


#include "UI.h"
typedef int32_t int32;
class Shape : public View {
protected:
    GLuint vertexBufId;
    int32 numVerts=4;//changes according to shape;
    virtual void createBuffers();///numVerts Should be set Before this
public:
    Shape();
    Shape(int32_t numVerts);
    virtual void draw();
};
class Rect : public Shape{
public:
    Rect();
    virtual void setBounds(float startX, float startY, float width, float height) override ;
    virtual void draw() override ;

};

#endif //PHOTOFX_SHAPES_H
