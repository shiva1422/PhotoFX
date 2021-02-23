//
// Created by Shiva Shankar patel psy on 12/7/20.
//

#ifndef PHOTOFX_MAIN_H
#define PHOTOFX_MAIN_H

#include "Commons.h"
#include "Editing.h"
#include "android/log.h"
enum EMenuType{OPTIONS_MENU,SUBOPTIONS_MENU};
class View;
class ImageViewStack;
class GlobalData//defs in GlobalData.cpp
{
private:
    ImageViewStack *optionMenu = nullptr,*subOptionsMenu=nullptr;
public:
    Editor *editor=nullptr;
    AppContext *appContext;
    DisplayParams *displayParams;
    View *contentView=NULL;//can be View &contentView;//move to appcontext;
    GLuint UIProgram;
    GLuint frameBufId=0;
    float sliderValueTest=0.0f;
    void menuItemChanged();
    void setEditingContext(Editor *editor){ this->editor=editor;}
    void setMenu(ImageViewStack *imageViewStack,EMenuType);

};

#endif //PHOTOFX_MAIN_H
