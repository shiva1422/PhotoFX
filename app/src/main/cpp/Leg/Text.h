//
// Created by Shiva Shankar patel psy on 3/18/21.
//

#ifndef PHOTOFX_TEXT_H
#define PHOTOFX_TEXT_H
#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "Commons.h"
#define textLog(...)((void)__android_log_print(ANDROID_LOG_ERROR,"TEXT RENDERING",__VA_ARGS__))
///most things can be static as app only needs one instance of TExtEngine;
////does text require rgba for each pixel ? if not why waste memory only gray value and alpha(maybe) is enough
class TextEngine{
private:
    FT_Library ftLibrary;//handle to the instance of Library//make static if need only one for entire app.
    FT_Face fontFace;
    int fontHeight=20,fontWidth=20;
    const char *defaultFont="/system/fonts/DroidSans.ttf";
public:
    status init();
    status setTextSize(int  width,int height);//WIDTH AND HEIGHT IN POINTS
    status setFont(const char* fontFile);
    void getImageFromString(const char *string,Bitmap *bitmap);

};


#endif //PHOTOFX_TEXT_H
