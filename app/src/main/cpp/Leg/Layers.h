//
// Created by Shiva Shankar patel psy on 4/10/21.
//

#ifndef PHOTOFX_LAYERS_H
#define PHOTOFX_LAYERS_H


#include "UI.h"
#include "EditableImage.h"

class Layer : public View{
private:
    EditableImage *editableImages= NULL;
   // void add(EditableImage *editableImage);//// Later Can add other Views as well (use View * instead);
public:
    virtual void draw() override ;
    void addEditableImage(EditableImage *editableImage);
    void process();
    EditableImage* getEditableImages(){return editableImages;}
    Layer(View *pView):View(pView)
    {
        editableImages=nullptr;
    }
};


#endif //PHOTOFX_LAYERS_H
