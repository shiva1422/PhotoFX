//
// Created by Shiva Shankar patel psy on 2/20/21.
//

#include <cstring>
#include "EditableImage.h"
#include "Main.h"
EditableImage::EditableImage(float startX, float startY, float Width, float height, Bitmap *image):ImageView(startX,startY,width,height,image)
{/////set buffer to 0 or noise is draw or draw input to ouput firsttime.
    outputBuffer.setDims(image->width, image->height);
    outputBuffer.configureColorBuffer();
    //outputBuffer.configureDepthBuffer();////optional remove if not needed
    outputBuffer.configure();
    outputTexId=outputBuffer.getTexId();
    inputTexId=texId;//two lines so that by default super draw method draw ouput;based on need we can change them to draw ouptu input or both.
    texId=outputTexId;//since these texId all beolong to single object exchnaging them is no problem (also input output are same size;
}
void EditableImage::initHistogramBuffer()
{
    static bool isBufferInitialized=false;
    if(!isBufferInitialized)
    {
        glGenBuffers(1,&histogramBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,256*sizeof(int32_t),(void * )0,GL_DYNAMIC_COPY);
        int32_t *bindata=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
        if(bindata)
        {
            Loge("HistogramEqualization","success buffer;.;;;;;;;;;;;;;");
            memset(bindata,0,256*sizeof(int32_t));
            for(int i=0;i<255;i++)
                Loge("histogram buffer value  ","the value at %d is %d",i,bindata[i]);
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        Graphics::printGlError("HistoGramEqualization");
        isBufferInitialized=true;
    }
    //to read or write ssbo
    //step1
    GLuint blockIndex=0;
    blockIndex=glGetProgramResourceIndex(GlobalData::getProgramId(),GL_SHADER_STORAGE_BLOCK,"bins");///may not be in curren program;
    //step 2 coonec block to ssbo tell shader on which binding oint it can find ssbo (2);
    GLuint bindingPoint=2;//same as in shader;
   // glBindBufferBase(GL_SHADER_STORAGE_BUFFER,bindingPoint,ssboId);
    //glUniformBlockBinding()
}
void EditableImage::computeHistogram()
{
    //just for printing;

   // glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,histogramBuffer);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER,histogramBuffer);
    int32_t *bindata=(int32_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int32_t),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    if(bindata)
    {
       // memset(bindata,0,256*sizeof(int32_t));
        for(int i=0;i<255;i++)
            Loge("histogram buffer value  ","the value at %d is %d",i,bindata[i]);
    } else
    {
        Loge("computeHis","could not map");
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

}
void EditableImage::drawHistogram()
{
    glBindBuffer(GL_ARRAY_BUFFER,histogramBuffer)
    glEnableVertexAttribArray(0);
}