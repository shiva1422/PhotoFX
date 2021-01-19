//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#ifndef PHOTOFX_UI_H
#define PHOTOFX_UI_H
#include "Graphics.h"
#endif //PHOTOFX_UI_H
#define UILogE(...)((void)__android_log_print(ANDROID_LOG_ERROR,"UILOG",__VA_ARGS__))
#define UILogI(...)((void)__android_log_print(ANDROID_LOG_INFO,"UILOG",__VA_ARGS__))
typedef struct Color{
    float red=0.0,green=0.0,blue=0.0,alpha=1.0;//use this check if converte to class needed;
}Color;
typedef struct Texture{
    GLuint texBufId=0,texId=0;
}Texture;

void InitializeUI();
class View{
    //////displayParams should be set first;
protected:
    float startX,startY,width,height;
    float r=0.5,g=0.5,b=0.5,a=1.0;
    float backgroundColor[4];
    float vertices[8];///check can be removed as its stored on gpuside in View.

public:
    static DisplayParams displayParams;
    View(){}
    ~View(){}///////clear buffers if there
    View(float startX,float startY,float width,float height)
    {
        setBounds(startX,startY,width,height);
    }
    View(float startX,float startY,float width,float height,bool deviceIndependent)
    {
        if(deviceIndependent)
            setBoundsDeviceIndependent(startX,startY,width,height);
        else
            setBounds(startX,startY,width,height);
    }
    float getStartX(){return startX;};
    float getStartY(){return startY;};
    float getWidth(){return width;};
    float getHeight(){return height;};
    float endX(){return startX+width;};
    float endY(){return startY+height;};
    float centerX(){return (startX+width/2);}
    float centreY(){return (startY+height/2);}
    float* getVertexAddr(){return vertices;}
    virtual void setBounds(float startX, float startY, float width, float height);
    void setBoundsDeviceIndependent(float startX,float startY,float width,float height);//just width and height in dp
    void fitToCentre(float boxStartX,float boxStartY,float boxWidth,float boxHeight);
    void fitToCentre(const View &view);
    static void setDisplayParams(DisplayParams displayParams1){displayParams=displayParams1;};
    void setBackgroundColor(float red,float green,float blue,float alpha){r=red,g=green,b=blue,a=alpha;};
   // void setBackgroundColor(float *pixel);


    virtual void draw(){clearRect();};////change ClearRect
    void clearRect();


    bool isTouched(float touchX,float touchY){
        return touchX >= startX && touchY >= startY && touchX <= (startX + width) &&
               touchY <= (startY + height);
    };
};
////////Destructors
class ImageView : public View{
private:

    bool isTextureSet=false;
    Bitmap *image;
protected:
    GLuint texId=0,texBufId=0,vertexBufId=0;

public:
    static GLuint texCoodBufId,indexBufId;///remove static in if these vary for each image
    static Bitmap defaultImage;
    ImageView();
    ImageView(float startX,float startY,float width,float height);
    ImageView(float startX,float startY,float width,float height,Bitmap *image);
    void setBounds(float startX, float startY, float width, float height);
   // void setupBuffers();
    ImageView(Bitmap *image){}
    void setTexture(Bitmap *image);
    static Texture createTexture(Bitmap *image);
    virtual void draw() override;

public:

};
class ImageViewStack : public View{
//ALL IMAGES SHOULD BE SAME SIZE BEFORE UPLOADING TO THE TEXTUREARRAY.
private:
    GLuint texId=0,vertexBufId=0;
    uint numViews=0;
    float viewGap=50.0f;//in pixels
    float singleImageWidth=0.0f;

public:
    ImageViewStack();
    ImageViewStack(uint numViews,int32_t imageWidth,int32_t imageHeight);
    void setBounds(float startX, float startY, float width, float height);
    void setNoViews(uint numViews,int32_t imageWidth,int32_t imageHeight);//make private accordtly with default Constructor.
    virtual void draw() override ;
};

