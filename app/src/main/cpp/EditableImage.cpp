//
// Created by Shiva Shankar patel psy on 2/20/21.
//

#include "EditableImage.h"
EditableImage::EditableImage(float startX, float startY, float Width, float height, Bitmap *image):ImageView(startX,startY,width,height,image)
{
    outputBuffer.setDims(image->width, image->height);
    outputBuffer.configureColorBuffer();
    outputBuffer.configureDepthBuffer();////optional remove if not needed
    outputBuffer.configure();
    ouputTexId=outputBuffer.getTexId();
    inputTexId=texId;//two lines so that by default super draw method draw ouput;based on need we can change them to draw ouptu input or both.
    texId=ouputTexId;//since these texId all beolong to single object exchnaging them is no problem (also input output are same size;
}
