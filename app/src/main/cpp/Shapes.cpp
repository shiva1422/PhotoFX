//
// Created by Shiva Shankar patel psy on 4/10/21.
//

#include "Shapes.h"
#include "UIShaderConstantsExtern.h"
Shape::Shape()
{

}
Shape::Shape(int32_t numVerts)
{
    this->numVerts=numVerts;
    createBuffers();
}
void Shape::createBuffers()
{
    glGenBuffers(1,&vertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVerts * 2, (void *)0, GL_STATIC_DRAW);///dimensions should be set before this or else reset dimesnion with same dims
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void Shape::setFillType(ShapeFillType fillType)
{
    this->fillType=fillType;
}
void Shape::draw()
{
    //glLineWidth(3.0);
    glUniform4f(glGetUniformLocation(AppContext::UIProgram,"uniformColor"),r,g,b,a);//cache the location
    glUniform1i(DRAWTYPELOC,SHAPEDRAWTYPE);
    glEnableVertexAttribArray(POSITIONATTRIBLOC);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glVertexAttribPointer(POSITIONATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 0,(void *)0);
    if(fillType==FILLTYPE_FILL)
    glDrawArrays(GL_TRIANGLE_FAN,0,numVerts);////draw as triangle strips;
    else
        glDrawArrays(GL_LINE_LOOP,0,numVerts);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

//Rectangle

Rect::Rect() : Shape(4)
{
}
void Rect::setBounds(float startX, float startY, float width, float height)
{
    Shape::setBounds(startX,startY,width,height);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    float *vertices=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*numVerts*2,GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);//is this needed instead lazy draw would be better;
    if(vertices)
    {
        ///X's
        vertices[0] = -1.0 + (startX * 2.0) /(float) Graphics::displayParams.screenWidth;//*2 so that-1 to 1 else we get 0 to 1 after conversion  leftX
        vertices[2] = -1.0 + ((startX + width) * 2.0) / (float) Graphics::displayParams.screenWidth;//rightX
        vertices[4] = vertices[2];
        vertices[6] = vertices[0];
        ///Y's
        vertices[1] = 1.0 - ((startY + height) * 2.0) / (float) Graphics::displayParams.screenHeight;//bottomy
        vertices[3] = vertices[1];
        vertices[5] = 1.0 - ((startY) * 2.0) / (float) Graphics::displayParams.screenHeight;//topy
        vertices[7] = vertices[5];
    }
    else
    {////lazy draw on
        ///uploading vertices everydrawcall
        UILogE("Rect::Bounds:,-error uploading vertices");
        Graphics::printGlError("Rect::setBouds()");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    glBindBuffer(GL_ARRAY_BUFFER,0);

}
Polygon::Polygon(int32 numVerts):Shape(numVerts)
{

}
Polygon::Polygon(float centreX, float centreY, float sideLengthOrRadius,float numVerts):Polygon(numVerts)
{
    this->centreX=centreX;
    this->centreY=centreY;
    this->sideLength=sideLength;
}
void Polygon::setSideLength(float sideLength)
{
    this->sideLength=sideLength;
    calculateVertices();

}
void Polygon::setBounds(float startX, float startY, float width, float height)
{
    Shape::setBounds(startX,startY,width,height);
    centreX=startX+width/2.0;
    centreY=startY+height/2.0;
    if(width<height)//////try keep width and heigh same and consider screenDensity;
    {
        sideLength=width/2.0;
    } else
        sideLength=height/2.0;
    calculateVertices();

}
void Polygon::setRotation(float angleInDegrees)
{
    this->rotation=angleInDegrees;
    calculateVertices();
}
void Polygon::calculateVertices()
{
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    float *vertices=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*numVerts*2,GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);//is this needed instead lazy draw would be better;
    if(vertices)
    {
        float angle=360.0/numVerts;
        for(int i=0;i<numVerts;i++)
        {
            ////calculating only 1/4 of num vertices is enough others can be mirrored in x y;
            float tempAngle=i*angle+rotation;//(i*angle*PI)/180)
            tempAngle=tempAngle*PI/180.0;
            vertices[2*i]=-1.0+((centreX+sideLength*cos(tempAngle))*2.0/displayParams.screenWidth);
            vertices[2*i+1]=centreY+sideLength*sin(tempAngle);
            vertices[2*i+1]=1.0-(vertices[2*i+1]*2.0)/(float)displayParams.screenHeight;
        }
    }
    else
    {////lazy draw on
        ///uploading vertices everydrawcall
        UILogE("PolyGon::Bounds:,-error uploading vertices");
        Graphics::printGlError("Rect::setBouds()");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
Capsule::Capsule(int32 numVertsPerSemiCircle) :Shape(2*numVertsPerSemiCircle)//to make num verts positive;
{

}
void Capsule::setBounds(float startX, float startY, float width, float height)
{
   Shape::setBounds(startX,startY,width,height);
   radius=height/2.0;
   calculateVerts();
}
void Capsule::calculateVerts()
{
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    float *vertices=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*numVerts*2,GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);//is this needed instead lazy draw would be better;
    if(vertices)
    {
        float tempCentreX=startX+width-radius;///instead of adding to temCentreX in loop try subtract from tempEndY(startX+width) gives correct when total verts=4(rectangel);;
        float tempCentreY=startY+radius;
        int tempNumVerts=numVerts/2;//for semicircle
        float angle=180.0/(tempNumVerts-1);//semicircle
        int loopCount=(tempNumVerts+1)/2;//to calculate only half semicircle and mirror them;+1 so that to include vertic if odd count
        for(int i=0;i<loopCount;i++)
        {
            float tempAngle=i*angle-90.0;//just so that starting at right semicircle bottom vert;
            tempAngle=tempAngle*PI/180.0;
            float xDis=radius*cos(tempAngle);
            float yDis=radius*sin(tempAngle);
            //4th quadrant;
            vertices[2*i]=-1.0+((tempCentreX+xDis)*2.0)/Graphics::displayParams.screenWidth;
            vertices[2*i+1]=1.0-((tempCentreY+yDis)*2.0)/Graphics::displayParams.screenHeight;

            //1st quadrant:
            vertices[2*(tempNumVerts-1-i)]=vertices[2*i];
            vertices[2*(tempNumVerts-1-i)+1]=1.0-((tempCentreY-yDis)*2.0)/Graphics::displayParams.screenHeight;

            //2rd qudrant -left semi circle;
            vertices[2*(tempNumVerts+i)]=-1.0+((startX+radius-xDis)*2.0)/Graphics::displayParams.screenWidth;
            vertices[2*(tempNumVerts+i)+1]=vertices[2*(tempNumVerts-1-i)+1];

            //3rd qudrant -left semi circle;
            vertices[2*(2*tempNumVerts-1-i)]= vertices[2*(tempNumVerts+i)];
            vertices[2*(2*tempNumVerts-1-i)+1]=vertices[2*i+1];

        }
    }
    else
    {////lazy draw on
        ///uploading vertices everydrawcall
        UILogE("PolyGon::Bounds:,-error uploading vertices");
        Graphics::printGlError("Rect::setBouds()");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    glBindBuffer(GL_ARRAY_BUFFER,0);
}