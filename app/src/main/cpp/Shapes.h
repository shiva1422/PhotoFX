//
// Created by Shiva Shankar patel psy on 4/10/21.
//

#ifndef PHOTOFX_SHAPES_H
#define PHOTOFX_SHAPES_H

#define PI 3.141592653589793238
#include "UI.h"
typedef int32_t int32;
enum ShapeFillType{FILLTYPE_EDGE,FILLTYPE_FILL};
class Shape : public View {
protected:
    ShapeFillType fillType=FILLTYPE_FILL;
    GLuint vertexBufId;
    int32 numVerts=4;//changes according to shape;
    virtual void createBuffers();///numVerts Should be set Before this
public:
    Shape();
    Shape(int32_t numVerts);
    void setFillType(ShapeFillType fillType);
    virtual void draw();
};
class Rect : public Shape{
public:
    Rect();
    virtual void setBounds(float startX, float startY, float width, float height) override ;

};
class Polygon : public Shape{
    //A regular polygon; if vertices are high enough get  a circle//sidelength=radius for circle
protected:
    float sideLength=0.0,centreX=0.0,centreY=0.0;
    float rotation=0;
    void calculateVertices();
public:
    Polygon(int32 numVerts);
    Polygon(float centreX,float centreY,float sideLenghtOrRadius,float numSides);
    float getRadius(){return sideLength;}
    void setRotation(float angleInDegrees);
    void setSideLength(float sideLength);
    virtual void setBounds(float startX,float startY,float width,float height) override ;

};
class Capsule : public Shape{
private:
    float radius=0.0;
    void calculateVerts();

public:
    Capsule(int32 numVertsPerSemiCircle);//
    virtual void setBounds(float startX,float startY,float width,float height) override ;



};

#endif //PHOTOFX_SHAPES_H
