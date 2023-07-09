//
// Created by Shiva Shankar patel psy on 3/18/21.
//

#include "Text.h"
status TextEngine::init()
{
    FT_Error fterror=FT_Init_FreeType(&ftLibrary);
    if(fterror!=0)
    {
        textLog("FATAL-ERROR : ERROR INITIALIZING FREETYPE LIBRARY");
        return STATUS_KO_FATAL;
    }
    textLog("successfully initialized freetype");
    FT_Int major, minor, patch;
    FT_Library_Version(ftLibrary, &major, &minor, &patch);
    textLog("FreeType's version is %d.%d.%d\n", major, minor, patch);
    if(setFont(defaultFont)==STATUS_KO_FATAL)
        return STATUS_KO_FATAL;
    return setTextSize(fontWidth,fontHeight);
}
status TextEngine::setTextSize(int width, int height)
{

    FT_Error  fterror =FT_Set_Char_Size(fontFace,0*64,height*64,AppContext::displayParams.xdpi,AppContext::displayParams.ydpi);// FT_Set_Pixel_Sizes(fontFace, 0, 64);
    if(fterror)
    {
        textLog("FATAL ERROR : COULD NOT SET TEXT SIZE =>RETRY OR .........");
        return STATUS_KO_FATAL;
    }
    fontHeight=height;
    fontWidth=width;
    return STATUS_OK;
}
status TextEngine::setFont(const char *fontFile)
{
    FT_Error fterror;
    fterror=FT_New_Face( ftLibrary,fontFile,0,&fontFace );///////see face index why 0;

    if (fterror == FT_Err_Unknown_File_Format )
    {
        textLog("FATAL-ERRROR : UNKNOWN FONT FILEFORMAT =>RESET FONT WITH RIGHT FILE FORMAT");
        return STATUS_KO_FATAL;
    }
    else if(fterror)
    {
        textLog("FATAL-ERRROR : ERROR LOADING FONT  =>RESET FONT WITH RIGHT FONT FILE");
        return STATUS_KO_FATAL;
    }
    textLog("font set successfully");
    return STATUS_OK;
}
void TextEngine::getImageFromString(const char *string, Bitmap *bitmap)
{
    //pixel_size = point_size * resolution / 72//max_advance_Width to quickly calculate advance width of string
    char ch;
    int ymax=0,ymin=0;
    int stringLength=strlen(string),totalStringWidth=0,stringHeight=0;
    FT_Error fterror;
    FT_Int32 render_flags = FT_RENDER_MODE_NORMAL;
    FT_Glyph glyphs[stringLength];
    FT_BitmapGlyph bitmaps[stringLength];
    FT_BBox box;
    FT_UInt glyph_index;
    for(int i=0;i<stringLength;i++)
    {
        ch=string[i];
        glyph_index = FT_Get_Char_Index(fontFace,ch);
        fterror = FT_Load_Glyph(fontFace, glyph_index, FT_LOAD_DEFAULT);
        if (fterror)
        {
            textLog("error loading glyph");
            return;
        }
        fterror =FT_Get_Glyph(fontFace->glyph,glyphs+i);// FT_Render_Glyph(fontFace->glyph,FT_RENDER_MODE_NORMAL);//use renderflages oabove
        if (fterror)
        {
            textLog("error getting glyph");
            return;
        }
        fterror = FT_Glyph_To_Bitmap(glyphs+i,FT_RENDER_MODE_NORMAL,NULL,0);
        if (fterror)
        {
            textLog("errorConverting glyph to bitmap");
            return;
        }
        totalStringWidth+=(glyphs[i]->advance.x>>10)/64;//if used kerning add that distance too
        FT_Glyph_Get_CBox( glyphs[i], FT_GLYPH_BBOX_PIXELS, &box );
        if(box.yMin<ymin)///TO GET THE RIGHT HEIGHT OF THE STRING
        {
            ymin=box.yMin;

        }
        if(box.yMax>ymax)
        {
            ymax=box.yMax;
        }
        textLog("the bdfdf is %d,%d,%d",box.xMax,box.xMin,box.yMin);
        bitmaps[i]=(FT_BitmapGlyph)glyphs[i];
    }

    //RENDERING STARTS FROMHERE TO GIVE PIXMAP
    stringHeight=ymax-ymin;
    textLog("the height is %d",stringHeight);
    if(bitmap->pixels != NULL)
    {
        free(bitmap->pixels);
        bitmap->pixels=NULL;
    }
    bitmap->pixels=(uint8_t *)malloc(totalStringWidth * (stringHeight) * sizeof(uint32_t));//stringHeight+1 cuz -topbearing in docs
    if(bitmap->pixels != NULL)
    {
        bitmap->width=totalStringWidth;bitmap->height=stringHeight;
        textLog("The totla width and height are %d and %d", bitmap->width, bitmap->height);
        uint penX = 0, penY = stringHeight+ymin;//penY=stringHeight-1;if(previously not +1)
        unsigned char *pixels = (unsigned char *) bitmap->pixels;
        memset(pixels, 0, totalStringWidth * stringHeight * sizeof(uint32_t));
        unsigned char *buffer;
        int noRows,pitch,pixmapIndex=0,bitW,bufferIndex=0;
        for (int i = 0; i < stringLength; i++)
        {
            penX += bitmaps[i]->left;//next bitmap
            penY -= bitmaps[i]->top;
            buffer = bitmaps[i]->bitmap.buffer;//glyphs buffer
            noRows = bitmaps[i]->bitmap.rows;
            bitW = bitmaps[i]->bitmap.width;
            pitch = bitmaps[i]->bitmap.pitch;
            for (int h = 0; h < noRows; h++)
            {
                for (int w = 0; w < bitW; w++)
                {
                    pixmapIndex = (penY + h) * totalStringWidth * 4 + (penX + w) * 4;
                    bufferIndex = (h * pitch) + w;
                    {
                        if( pixels[pixmapIndex + 1]==0)//cuz already filled overlaps chaeck any not only 1;//cuz gy are overlapping improve later with kerning;
                        {
                            pixels[pixmapIndex] = buffer[bufferIndex] ;
                            pixels[pixmapIndex + 1] = buffer[bufferIndex];
                            pixels[pixmapIndex + 2] = buffer[bufferIndex];
                            pixels[pixmapIndex + 3] = buffer[bufferIndex];
                            //textLog("pixel value r %d",pixels[pixmapIndex+3]);
                        }
                    }
                }
            }
            penX += (glyphs[i]->advance.x >> 10) / 64-bitmaps[i]->left;//cuz length-1 coords
            // penX+=bitW;
            penY = stringHeight+ymin;//-1
            FT_Done_Glyph(glyphs[i]);

        }


    }
}