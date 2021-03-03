//
// Created by Shiva Shankar patel psy on 2/20/21.
//

#ifndef PHOTOFX_EDITABLEIMAGE_H
#define PHOTOFX_EDITABLEIMAGE_H


#include "OffScreenRender.h"
#include "UI.h"
////try to keep the editable image to only rgb and single alpha for whole image and add aplha only when required(save memory).

class EditableImage : public ImageView{ //the layer class consists of EditableImages
private:
    GLuint ouputTexId=0,inputTexId=0;//imageView texId is set to whatever we want to draw inputTexId is for storing default texId
    uint imageToDraw=0;//1-to draw input ,0 to draw ouput ,2for both;


public:
    FrameBuffer outputBuffer;//stores output of image above after editing;//default const create id so just set dims and configure;
    EditableImage();
  //  EditableImage(ImageView *inputImage){}
    EditableImage(float startX,float startY,float Width,float height,Bitmap *image);
    GLuint getActiveTexId(){return texId;}//could be input or outputtexid
    GLuint getInputTexId(){return inputTexId;}
    GLuint getOutputTexId(){return ouputTexId;}
    //virtual void draw() override
    void drawInput() ;
    void drawOuput() ;

};


#endif //PHOTOFX_EDITABLEIMAGE_H