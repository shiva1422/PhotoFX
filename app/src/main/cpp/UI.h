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
    ~View();///////clear buffers if there
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
    kforceinline float getStartX(){return startX;};
    kforceinline float getStartY(){return startY;};
    kforceinline float getWidth(){return width;};
    kforceinline float getHeight(){return height;};
    kforceinline float endX(){return startX+width;};
    kforceinline float endY(){return startY+height;};
    kforceinline float centerX(){return (startX+width/2);}
    kforceinline float centreY(){return (startY+height/2);}
    //float* getVertexAddr(){return vertices;}
    void setVisibility(bool shouldBeVisible){visible=shouldBeVisible;}
    bool getVisibility(){return visible;}
    void setBoundsDeviceIndependent(float startX,float startY,float width,float height);//just width and height in dp
    void fitToCentre(float boxStartX,float boxStartY,float boxWidth,float boxHeight);
    void fitToCentre(const View &view);
    static void setDisplayParams(DisplayParams displayParams1){displayParams=displayParams1;};
    void setBackgroundColor(float red,float green,float blue,float alpha){r=red,g=green,b=blue,a=alpha;};
    bool isPointInside(float x,float y);
    bool isPointYInside(float y);
    bool isPointXInside(float x);
    bool isPointToTheRight(float x);
    bool isPointToTheLeft(float x);
    bool isPointAbove(float y);
    bool isPointBelow(float y);
   // void setBackgroundColor(float *pixel);
    virtual void setBounds(const View *view)
   {
        setBounds(view->startX,view->startY,view->width,view->height);
   }
    void setStartX(float startX);
    void setEndX(float endX);
    void setStartY(float startY);
    void setEndY(float endY);
    virtual void setBounds(float startX, float startY, float width, float height);
    virtual void moveHorizontalByDistance(float moveDistanceX);
    virtual void draw(){clearRect();};////change ClearRect////shoud draw be protected?
    void clearRect();

    //Touch:
   virtual bool isTouched(float touchX,float touchY,int pointerId,TouchAction touchAction);
   virtual void setOnTouchListener(OnTouchListener *touchListener)
   {
       this->onTouchListener=touchListener;///freeing the old one and current parameter check
   }
   virtual void setOnClickListener(OnClickListener *clickListener)
   {
       this->onTouchListener=clickListener;
   }
   //virtual void setTouchFunction(bool (*touchFunction)(float touchX,float touchY,TouchAction));
};
////////Destructors
class ImageView : public View{
private:

    bool isTextureSet=false;
protected:
    Bitmap image;
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
    void setTexture(const char *assetLoc,bool shouldFitToPreviousBounds);
    GLuint getTextureId(){return this->texId;}
    void setTextureId(GLuint texId);
    static Texture createTexture(Bitmap *image);
    virtual void draw() override;



};
class ImageViewStack : public View{//only one view is active (highlighetd at a time)
//ALL IMAGES SHOULD BE SAME SIZE BEFORE UPLOADING TO THE TEXTUREARRAY.
private:
    GLuint texId=0,vertexBufId=0;
    int numViews=0;
    int startViewNo=0;
    int numViewsToDraw=0.0;
    float viewGap=2.0f;//in pixels
    float singleImageWidth=0.0f,singleImageHeight;//for draw and touch final dims
    float bitmapWidth=0.0f,bitmapHeight=0.0f;
    int activeViewNo=INT32_MAX;//problem?

    void setSingleViewBounds();


public:
    ImageViewStack();
    ImageViewStack(int numViews,int32_t imageWidth,int32_t imageHeight);
    void setBounds(float startX, float startY, float width, float height) override ;
    void setNoViews(int numViews,int32_t imageWidth,int32_t imageHeight);//make private accordtly with default Constructor.
    void setNoViewsVisible(int drawCount){this->numViewsToDraw=drawCount;}
    void fitViewsInBounds();
    void fitViewsInBoundsDP();
    int getViewNoAtLoc(float x,float y);
    void setActiveViewNo(int viewNo){this->activeViewNo=viewNo;}
    void setTextureForViewNo(int viewNo,const  char* assetLoc);
    void fillTexture(int viewNo,Bitmap *bitmap);
    int getActiveViewNo(){return activeViewNo;}
    virtual void draw() override ;
};
class Shape;
class SliderSet : public View{
private:
    Shape *baseLineView,*pointerView;
    float value=0.0f;//0.0 to 1.0
public:
    static uint sliderCounter;
    uint sliderNo=sliderCounter++;////////increment in constructor
    SliderSet();
    virtual void setBounds(float startX, float startY, float width, float height) override ;
    float getVaule(){return value;}
    void setPointerLoc(float x,float y);
    void reset();
    virtual void draw() override ;
};
class ViewGroup : public View{
private:
    uint noViews=0;
    uint defaultSize=15;//default no ints;
    View **views=nullptr;
protected:
public:
    ViewGroup();
    void addView(View  *view);
    virtual void draw() override ;
    virtual bool isTouched(float touchX,float touchY,int pointerId,TouchAction touchAction);


};




#endif //PHOTOFX_UI_H
