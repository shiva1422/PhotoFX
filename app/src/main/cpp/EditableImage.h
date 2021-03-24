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
    GLuint outputTexId=0,inputTexId=0,binsTexIds[2]={0,0};//imageView texId is set to whatever we want to draw inputTexId is for storing default texId
    uint imageToDraw=0;//1-to draw input ,0 to draw ouput ,2for both;


public:
    /////////User Ssbos instead of fbos(arm doc compute shaders)ShaderImages
    int32_t binsSize=360;//256 would be enough but for hue eq 360 required;
    bool isHistogramCalculated=false;
    GLuint binsBuffers[2]={0,0};//one for bins and one for cdf of bins;
    FrameBuffer outputBuffer;//stores output of image above after editing;//default const create id so just set dims and configure;
    EditableImage();
  //  EditableImage(ImageView *inputImage){}
    EditableImage(float startX,float startY,float Width,float height,Bitmap *image);
    GLuint getActiveTexId(){return texId;}//could be input or outputtexid
    GLuint getInputTexId(){return inputTexId;}
    GLuint getOutputTexId(){return outputTexId;}
    //virtual void draw() override
    void drawInput() ;
    void drawOuput() ;
    int getImageWidth(){ return image->width;}
    int getImageHeight(){return image->height;}

    ///Move histogram works to separate file;
    void createHistogramTextures();//for bins and binsCDF not for input and output images
    void computeHistogram(int forWhat);//for what should be same as in auto shader//replace for what with enum
    void resetHistogram();
    void drawHistogram(int forWhat);
    void showHistogramValues(int forWhat);//same as computeParam

};


#endif //PHOTOFX_EDITABLEIMAGE_H
