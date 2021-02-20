//
// Created by Shiva Shankar patel psy on 2/20/21.
//

#ifndef PHOTOFX_EDITABLEIMAGE_H
#define PHOTOFX_EDITABLEIMAGE_H


#include "OffScreenRender.h"
#include "UI.h"

class EditableImage : public ImageView{ //the layer class consists of EditableImages
private:
    GLuint ouputTexId=0;
public:
    FrameBuffer secondBuffer;//stores output of image above after editing;
    EditableImage();
    EditableImage(ImageView *inputImage){}

};


#endif //PHOTOFX_EDITABLEIMAGE_H
