//
// Created by Shiva Shankar patel psy on 4/10/21.
//

#include "Layers.h"

void Layer::draw()
{
    clearRect();
    if(editableImages)//////so wrong why !
    {
        Loge("Layer draw","ther is image");
        editableImages->draw();

    }
}

void Layer::addEditableImage(EditableImage *editableImage)
{
    if(this->editableImages)///////clear previous image(ond destroy destructors clear buffers images);
    {
        delete this->editableImages;
        Loge("Layer ","delete old editableImage");
    }
    this->editableImages=editableImage;//for now only one;
}
void Layer::process()
{

}