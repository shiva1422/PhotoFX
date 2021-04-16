//
// Created by Shiva Shankar patel psy on 4/15/21.
//

#ifndef PHOTOFX_RECYCLERVIEW_H
#define PHOTOFX_RECYCLERVIEW_H

#include "Commons.h"
#include "UI.h"

class RecyclerView : public View{
protected:
    int32 numViews=0,startViewIndex=0,lastViewIndex=0,listStartIndex=0,maxLastListIndex=10;//maxLastListIndex =listCount-1;where the list is ouside data;
    float viewGap=5.0,viewWidth=0.0;
    View **views= nullptr;
    kforceinline int32 getListLastIndex(){return listStartIndex+numViews-1;}
    virtual void onInit();//to create views on custructions;
    virtual void onRecycle(bool right);
    void setViewsBounds(float startX, float startY, float width, float height);
public:
    RecyclerView();
    RecyclerView(int32 numViews);
    ~RecyclerView();
    virtual void setBounds(float startX, float startY, float width, float height) override ;
    virtual void draw() override ;
    void setBounds(View *view) ;
    void onMove(float moveDistanceX);
};

//Touch
class RecyclerTouchListener: public OnTouchListener{
private:
    float previousTouchX=0,previousTouchY=0;
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;


};
#endif //PHOTOFX_RECYCLERVIEW_H
