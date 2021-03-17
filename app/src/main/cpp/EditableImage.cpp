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
void EditableImage::drawIntensityHistrogram()
{
    //any buffer can be bound to ssbo;no matter how they were originally created
    static bool isBufferInitialized=false,isUpdateNeeded=true;
    GLuint ssboId=0;///SSBOS are similar to unifroms(UBO) with ability read write and unlimited size;check if shader and here sizes ok
    if(!isBufferInitialized)
    {
        glGenBuffers(1,&ssboId);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssboId);
        glBufferData(GL_SHADER_STORAGE_BUFFER,256*sizeof(int),(void *)0,GL_DYNAMIC_COPY);//use uint if shader support
        //if no error
       if( Graphics::printGlError("drawIntensity bufferinitialization")!=GL_OUT_OF_MEMORY)
        isBufferInitialized=true;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
    }
    if(isUpdateNeeded)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER,ssboId);
        int *p=(int *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER,0,256*sizeof(int),GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
        if(p)
        {
            memset(p,0,256*sizeof(int));
            isUpdateNeeded=false;
        }
        else
        {
            Loge("drawIntensityHistogram","could not MapBuffer");
        }
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        Graphics::printGlError("drawHistogram");

    }

    //to read or write ssbo
    //step1
    GLuint blockIndex=0;
    blockIndex=glGetProgramResourceIndex(GlobalData::getProgramId(),GL_SHADER_STORAGE_BLOCK,"bins");///may not be in curren program;
    //step 2 coonec block to ssbo tell shader on which binding oint it can find ssbo (2);
    GLuint bindingPoint=2;//same as in shader;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,bindingPoint,ssboId);
    //glUniformBlockBinding()
}
void EditableImage::compute()
{
    //TO decide how work shoulld be decide use workGroupsize and counts
    int workGroupCount[3],workGroupSize[3],maxInvocationSize;//count for dispatch,size for shader(threads);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,0,&workGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,1,&workGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,2,&workGroupCount[2]);
    Loge("GLobalWorkGroupsize  for dispatch", "theComputeGroupCount is %d and %d and %d", workGroupCount[0], workGroupCount[1], workGroupCount[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,0,&workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,1,&workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE,2,&workGroupSize[2]);
    Loge("maxLocalWorkGroup size in shaders", "theComputeGroupsize is %d and %d and %d", workGroupSize[0], workGroupSize[1], workGroupSize[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,&maxInvocationSize);
    Loge("MaxThreads supported per workgroup(product of thre localworkgroup sizes","the max invocationsiz is %d",maxInvocationSize);
    static bool isProgramCreated=false;
    static GLuint computeProgram;
    if(!isProgramCreated)
    {
        computeProgram=Shader::createComputeProgram(AppContext::getApp(),"Filters/computeShaders/testCompute.glsl");
        if(computeProgram)
        isProgramCreated=true;
    }
    GlobalData::useGlProgram(computeProgram);
    glBindImageTexture(0, outputTexId, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI);///////texture should be set before this
    glBindImageTexture(1, outputTexId, 0, GL_FALSE, 0, GL_WRITE_ONLY,GL_RGBA8UI);
    glDispatchCompute(this->image->width, this->image->height,1);
    //   printGlError("computing");
     glMemoryBarrier(GL_ALL_BARRIER_BITS);
    GlobalData::setDefaultGlProgram();
    Graphics::printGlError("EditableImage::Compute");
}