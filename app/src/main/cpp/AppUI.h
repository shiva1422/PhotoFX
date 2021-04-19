//
// Created by Shiva Shankar patel psy on 4/8/21.
//

#ifndef PHOTOFX_APPUI_H
#define PHOTOFX_APPUI_H


#include "UI.h"
#include "RecyclerView.h"
#include "Editing.h"

class AppUI{

private:
    ImageView *saveButton,*filesButton;

public:
    View frameBounds;//for main image in editor
    View topSection,optionsSection,subOptionsSection,slidersSection;
    View *options=nullptr,*subOptions=nullptr;
    ViewGroup contentView;//content
    View *sliderSets[5];
    static View& getFrameBounds();
    void init();
    void onOptionChanged(EOptions activeOption);
    void draw();
};

//options imageViewStack;
class Options : public ImageViewStack
{
public:
    Options(int numViews,float bitmapWidth,float bitmapHeight);
};

class OptionsClickListener: public ImageViewStackClickListener{
public:
    virtual bool onClick(float clickX,float clickY,ImageViewStack *stackView) override ;
};


//suboptions section recylcer views;
class AutoOptions:public RecyclerView
{
protected:
    virtual void onInit() override ;
    virtual void reportChangeOfActiveListIndex() override ;
public:
    AutoOptions(int numViews);
    AutoOptions(int numViews,int maxListCount);//maxListCount>=numViews;
};
///Click and Touch Handlers Below;
class SaveButtonClickListener : public OnClickListener{
public:
    virtual bool onClick(View *view) override;
};


#endif //PHOTOFX_APPUI_H
