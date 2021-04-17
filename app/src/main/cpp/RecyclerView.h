//
// Created by Shiva Shankar patel psy on 4/15/21.
//

#ifndef PHOTOFX_RECYCLERVIEW_H
#define PHOTOFX_RECYCLERVIEW_H

#include "Commons.h"
#include "UI.h"

class RecyclerView : public View{
protected:
    int32 numViews=0,startViewIndex=0,lastViewIndex=0,listStartIndex=0,maxLastListIndex=10,activeListIndex=INT32_MAX;//maxLastListIndex =listCount-1;where the list is ouside data;
    float viewGap=5.0,viewWidth=0.0;
    View **views= nullptr;
    kforceinline int32 getListLastIndex(){return listStartIndex+numViews-1;}
    kforceinline int32 getListIndexAtOffset(int viewIndexOffset)//taking startView as index 0
    {
        return listStartIndex+viewIndexOffset;
    }
    virtual void onInit();//to create views on custructions;
    virtual void onRecycle(bool right);
    void setViewsBounds(float startX, float startY, float width, float height);
    virtual void reportChangeOfActiveListIndex()=0;
public:
    RecyclerView();
    RecyclerView(int32 numViewsToDraw);
    ~RecyclerView();
    virtual void setBounds(float startX, float startY, float width, float height) override ;
    virtual void draw() override ;
    void setBounds(View *view) ;
    int32 setActiveListIndex(float x, float y);
    void onMove(float moveDistanceX);
};

//Touch
class RecyclerTouchListener: public OnTouchListener{
private:
    float previousTouchX=0,previousTouchY=0,initialTouchX=0,initialTouchY=0,totalMoveDisX=0.0;
    bool bClickCancel=true;
public:
    virtual bool onTouch(float touchX,float touchY,int pointerId,TouchAction touchAction,View *view) override ;


};
#endif //PHOTOFX_RECYCLERVIEW_H
