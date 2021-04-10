//
// Created by Shiva Shankar patel psy on 4/10/21.
//

#include "Shapes.h"
#include "UIShaderExtern.h"
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
void Shape::draw()
{
    View::draw();//should be implemented for shapes but by default draws this;
}

//Rectangle

Rect::Rect() : Shape(4)
{
}
void Rect::setBounds(float startX, float startY, float width, float height)
{
    Shape::setBounds(startX,startY,width,height);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    float *vertices=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*numVerts*2,GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
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
        UILogE("Rect::Bounds:,-error uploading vertices");
        Graphics::printGlError("Rect::setBouds()");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    glBindBuffer(GL_ARRAY_BUFFER,0);

}
void Rect::draw()
{
    glUniform4f(glGetUniformLocation(AppContext::UIProgram,"uniformColor"),r,g,b,a);//cache the location
    float color[4]={r,g,b,a};
    glUniform1fv(FLOATPARAMSLOC,4,color);
    glUniform1i(DRAWTYPELOC,SHAPEDRAWTYPE);
    glEnableVertexAttribArray(POSITIONATTRIBLOC);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glVertexAttribPointer(POSITIONATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 0,(void *)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ImageView::indexBufId);////cache it shapes instead ImageView;
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,(void *)0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

}