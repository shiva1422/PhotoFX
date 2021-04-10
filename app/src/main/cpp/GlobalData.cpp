//
// Created by Shiva Shankar patel psy on 2/19/21.
//
#include "Main.h"
#include "UI.h"
#include "Commons.h"
#include "JavaCalls.h"
#include "AppUI.h"

GLuint GlobalData::activeProgram=0,GlobalData::previousProgram=0;
GLuint GlobalData::UIProgram=0;
////header Main.h for this .cpp change later;
void GlobalData::menuItemChanged()
{
    /////instead of setting all options set only that changed using the parameter as the viewthat changed;
    if(editor)
    {
        Loge("GlobalData","menuitem changed");
        editor->setActiveOption(optionMenu->getActiveViewNo());
        editor->setActiveSubOption(subOptionsMenu->getActiveViewNo());
        /////reset all req inputViews(Sliders);
        if(optionMenu->getActiveViewNo()!=1)
        for(int i=0;i<SliderSet::sliderCounter;i++)
        {
            slider[i].reset();//check !null;
        }
    }
    else
        {
        Loge("GlobalData::muItemChanged Error","Editing Context Not set Yet");
        }
    if(optionMenu->getActiveViewNo()==1)
    {
        for(int i=0;i<4;i++)
        {
            slider[i].setVisibility(false);
        }
    }
    else
        {
            for(int i=0;i<4;i++)
            {
                slider[i].setVisibility(true);
            }
        }

}
void GlobalData::setMenu(ImageViewStack *imageViewStack, EMenuType menuType)
{
    switch(menuType)
    {
        case OPTIONS_MENU:
            optionMenu=imageViewStack;
            break;
        case SUBOPTIONS_MENU:
            subOptionsMenu=imageViewStack;
            break;
        default:
        {}

    }
}
void GlobalData::addInputComponent(SliderSet *sliderSet, EInputType inputType)
{
    switch (inputType)
    {
        case R_INPUT:
        {
            this->slider=sliderSet;

        }break;
        default:
        {}

    }
}
void GlobalData::onImportImage(int fd)///previous Image Should be cleared
{
    this->fd=fd;
    imageImportNeeded=true;


}
void GlobalData::importImage()
{
    importedImage=(Bitmap *)malloc(sizeof(Bitmap));/////all news check
    ::importImage(importedImage,fd);
    useGlProgram(UIProgram);
    EditableImage *importedEditableImage= NULL;
    try {
            importedEditableImage=new EditableImage(importedImage,appUI.frameBounds);
    }
    catch (std::bad_alloc& exc)
    {
        Loge("Catch error","bad alloc");
        importedEditableImage= NULL;
    }
    if(importedEditableImage)
    {
        editor->addEditableImage(importedEditableImage);
    }
    if(testImage)
    {
        Loge("oniMporg global data Success","sadfsdf width and height %d,%d",importedImage->width,importedImage->height);
      //  ViewGroup *viewGroup= dynamic_cast<ViewGroup *>(contentView);
        //viewGroup->addView(testImage);

    }
    glFinish();
    glFlush();

    //editor->editableImage=testImage;
    // editor->editableImage->onDestroy();///////previous image manage;
    // editor->editableImage=editableImage;
    // ((ViewGroup *)contentView)->addView(editableImage);
    usePreviousProgram();
    Graphics::printGlError("import image");
}
void GlobalData::onSaveImage()
{
    Loge("on Save Image()","image saved");
    saveImage();
}