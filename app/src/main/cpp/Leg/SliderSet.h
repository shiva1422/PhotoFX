//
// Created by pspr1 on 4/18/2021.
//

#ifndef PHOTOFX_SLIDERSET_H
#define PHOTOFX_SLIDERSET_H

#include "Shapes.h"

class SliderSet : public View{
protected:
    View *baseLineView,*pointerView;
    float value=0.0f;//0.0 to 1.0
public:
    static int sliderCounter;
    int sliderNo=sliderCounter++;////////increment in constructor
    SliderSet();
   virtual ~SliderSet();
    virtual void setBounds(float startX, float startY, float width, float height) override ;
    float getVaule(){return value;}
    void setPointerLoc(float x,float y);
    void reset();
    virtual void draw() override ;
};
class HueBar : public SliderSet{
public:
    HueBar();
};
class HueRange : public View
{
private:
    Shape *rangeLow= nullptr,*rangeHigh=nullptr;
    float lowerLimit=0.0,upperLimit=0.0;
    int activePointer;//0 for left 1 for right
private:
    void calculateUpperLimit();
    void calculateLowerLimit();
    float rangeToDistance(float limitValue);

public:
    HueRange();
    ~HueRange();
    virtual void setBounds(float startX, float startY, float width, float height) override ;
    bool onDispatchTouchToChildViews(float touchX,float touchY,int32 pointerId,TouchAction touchAction,float moveX);
    virtual void draw();

};

//Touch

class SliderTouchListener : public OnTouchListener{
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;

};
class HueRangeTouchListener: public OnTouchListener{
private:
    float previousTouchX=0,previousTouchY=0;
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;
};
#endif //PHOTOFX_SLIDERSET_H
