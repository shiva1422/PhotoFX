//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#ifndef PHOTOFX_UI_H
#define PHOTOFX_UI_H
#include "Graphics.h"
#include "EventHandling.h"
#define UILogE(...)((void)__android_log_print(ANDROID_LOG_ERROR,"UILOG",__VA_ARGS__))
#define UILogI(...)((void)__android_log_print(ANDROID_LOG_INFO,"UILOG",__VA_ARGS__))
typedef struct Color{
    float red=0.0,green=0.0,blue=0.0,alpha=1.0;//use this check if converte to class needed;
}Color;
typedef struct Texture{
    GLuint texBufId=0,texId=0;
}Texture;
class View;
class OnTouchListener;
typedef bool (View::*OnTouch)(float x,float y,TouchAction touchAction);//func nam Ontouch;
void InitializeUI();
class View{
    //////displayParams should be set first;
protected:
    float startX,startY,width,height;
    float r=0.5,g=0.5,b=0.5,a=1.0;
    float backgroundColor[4];
    //float vertices[8];///check can be removed as its stored on gpuside in View.
    bool visible=true;
 //Touch:
      OnTouchListener *onTouchListener=new ViewTouchListener() ;//free in destructor;


public:

    static DisplayParams displayParams;////move this to Graphics
    View();
    View(const View *view)
    {
        setBounds(view);
    }
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
    //float* getVertexAddr(){return vertices;}
    void setVisibility(bool shouldBeVisible){visible=shouldBeVisible;}
    bool getVisibility(){return visible;}
    void setBoundsDeviceIndependent(float startX,float startY,float width,float height);//just width and height in dp
    void fitToCentre(float boxStartX,float boxStartY,float boxWidth,float boxHeight);
    void fitToCentre(const View &view);
    static void setDisplayParams(DisplayParams displayParams1){displayParams=displayParams1;};
    void setBackgroundColor(float red,float green,float blue,float alpha){r=red,g=green,b=blue,a=alpha;};
   // void setBackgroundColor(float *pixel);
    virtual void setBounds(const View *view)
   {
        setBounds(view->startX,view->startY,view->width,view->height);
   }
    virtual void setBounds(float startX, float startY, float width, float height);
    virtual void draw(){clearRect();};////change ClearRect////shoud draw be protected?
    void clearRect();

    //Touch:
   virtual bool isTouched(float touchX,float touchY,int pointerId,TouchAction touchAction);
   virtual void setOnTouchListener(OnTouchListener *touchListener)
   {
       this->onTouchListener=touchListener;///freeing the old one and current parameter check
   }
   //virtual void setTouchFunction(bool (*touchFunction)(float touchX,float touchY,TouchAction));
};
////////Destructors
class ImageView : public View{
private:

    bool isTextureSet=false;
protected:
    Bitmap *image= nullptr;
    GLuint texId=0,texBufId=0,vertexBufId=0;///View class also needs vertexBufID(check moving there)


public:
    static GLuint texCoodBufId,indexBufId;///remove static in if these vary for each image
    static Bitmap defaultImage;
    ImageView();

    ImageView(float startX,float startY,float width,float height);
    ImageView(float startX,float startY,float width,float height,Bitmap *image);
    virtual void setBounds(float startX, float startY, float width, float height) override ;
   // void setupBuffers();
    ImageView(Bitmap *image){}
    void setBounds(ImageView *image)  ;
    void printVerts();
    void setTexture(Bitmap *image);
    GLuint getTextureId(){return this->texId;}
    void setTextureId(GLuint texId);
    static Texture createTexture(Bitmap *image);
    virtual void draw() override;



};
class ImageViewStack : public View{//only one view is active (highlighetd at a time)
//ALL IMAGES SHOULD BE SAME SIZE BEFORE UPLOADING TO THE TEXTUREARRAY.
private:
    GLuint texId=0,vertexBufId=0;
    uint numViews=0;
    uint startViewNo=0;
    int numViewsToDraw=0.0;
    float viewGap=10.0f;//in pixels
    float singleImageWidth=0.0f;
    uint activeViewNo=0;



public:
    ImageViewStack();
    ImageViewStack(uint numViews,int32_t imageWidth,int32_t imageHeight);
    void setBounds(float startX, float startY, float width, float height) override ;
    void setNoViews(uint numViews,int32_t imageWidth,int32_t imageHeight);//make private accordtly with default Constructor.
    void setNoViewsVisible(uint drawCount){this->numViewsToDraw=drawCount;}
    uint getViewNoAtLoc(float x,float y);
    void setActiveViewNo(uint viewNo){this->activeViewNo=viewNo;}
    uint getActiveViewNo(){return activeViewNo;}
    virtual void draw() override ;
};

class SliderSet : public View{
private:
    Bitmap *baseImage= nullptr,*pointerImage= nullptr;
    GLuint vertexBufId=0,baseTexId=0,pointerTexId=0;
    ImageView baseImageView,pointerImageView;
    float value=0.0f;//0.0 to 1.0

    bool isVisible=true;



public:
    static uint sliderCounter;
    uint sliderNo=sliderCounter++;////////increment in constructor
    SliderSet();
    void setBounds(float startX, float startY, float width, float height) override ;
    void setTexture(Bitmap *image);
    float getVaule(){return value;}
    void setPointerLoc(float x,float y);
    void reset();
    virtual void draw() override ;
};
class ViewGroup : public View{
private:
    uint noViews=0;
    uint defaultSize=10;//default no ints;
    View **views=nullptr;
protected:
public:
    ViewGroup();
    void addView(View  *view);
    virtual void draw() override ;
    virtual bool isTouched(float touchX,float touchY,int pointerId,TouchAction touchAction);


};



#endif //PHOTOFX_UI_H
