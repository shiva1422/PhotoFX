//
// Created by pspr1 on 4/17/2021.
//

#ifndef PHOTOFX_PHOTOAPP_H
#define PHOTOFX_PHOTOAPP_H

#include "Commons.h"
#include "Editing.h"
#include "android/log.h"
#include "JniMethods.h"
#include "AppUI.h"
#include "SliderSet.h"

enum EMenuType{OPTIONS_MENU,SUBOPTIONS_MENU};
enum EInputType{R_INPUT};
class View;
class ImageViewStack;
class PhotoApp//defs in PhotoApp.cpp/////global data
{
private:
    ImageViewStack *optionMenu = nullptr,*subOptionsMenu=nullptr;
    SliderSet *slider=nullptr;
    static  GLuint activeProgram,previousProgram;
public:
    EditableImage *testImage= nullptr;
    Bitmap *importedImage= nullptr;
    bool imageImportNeeded=false;
    int fd;
    AppUI appUI;

    bool activeHistogram=false;//test only
    Editor *editor=nullptr;
    AppContext *appContext;
    DisplayParams *displayParams;
    View *contentView=NULL;//can be View &contentView;//move to appcontext;
    static GLuint UIProgram;
    GLuint frameBufId=0;
    float sliderValueTest=0.0f;
    void menuItemChanged();
    void setEditingContext(Editor *editor){ this->editor=editor;}
    void setMenu(ImageViewStack *imageViewStack,EMenuType);
    void addInputComponent(SliderSet *sliderSet,EInputType);
    static void useGlProgram(GLuint programId)
    {    previousProgram=activeProgram;
        glUseProgram(programId);
        activeProgram=programId;
    };
    static void usePreviousProgram(){glUseProgram(previousProgram);};
    static GLuint  getProgramId(){return activeProgram;}
    static void setDefaultGlProgram(){glUseProgram(UIProgram);activeProgram=UIProgram;}
    void toggleProcessingType()
    {   if(editor)
            editor->toggleProcessingType();
        else
            Loge("ToggleProcessingType","cant as there is no valid editor");
    }
    void onOptionChanged(int optionNo);
    void onImportImage(int fd);
    void importImage();
    void onSaveImage();


};

#endif //PHOTOFX_PHOTOAPP_H
