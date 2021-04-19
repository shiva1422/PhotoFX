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
    uint sliderNo=sliderCounter++;////////increment in constructor
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

//Touch

class SliderTouchListener : public OnTouchListener{
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;

};
#endif //PHOTOFX_SLIDERSET_H
