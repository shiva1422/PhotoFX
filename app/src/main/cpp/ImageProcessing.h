//
// Created by Shiva Shankar patel psy on 2/13/21.
//

#ifndef PHOTOFX_IMAGEPROCESSING_H
#define PHOTOFX_IMAGEPROCESSING_H
#include "Graphics.h"
class ImageView;
class PhotoFX{
public:
    GLuint shaderProgram=0;
    GLuint inputTexId=0;
    GLuint ouputTexId=0;
    Bitmap *inputImage=nullptr;

public:
    static android_app *app;
    void apply();
    PhotoFX();
};
#endif //PHOTOFX_IMAGEPROCESSING_H
