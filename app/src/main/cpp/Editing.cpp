//
// Created by Shiva Shankar patel psy on 2/16/21.
//

#include <string>
#include "Editing.h"
#include "Main.h"

std::string ShaderManager::shadersFolder="Filters";
void Editor::process()
{
    srand(time(nullptr));
    static float param=1.0;
    glUseProgram(activeShaderId);
    GlobalData *globalData=(GlobalData *)((AppContext::getApp())->userData);
    glBindFramebuffer(GL_FRAMEBUFFER,editableImage->secondBuffer.getId());
    glViewport(0,0,editableImage->getHeight(),editableImage->getWidth());
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform1i(glGetUniformLocation(activeShaderId,"frameBuf"),1);
    glUniform1f(1,globalData->sliderValueTest);

    GLushort indices[6];
    indices[0]=0, indices[1]=1, indices[2]=2, indices[3]=2, indices[4]=3, indices[5]=0;
    float verts[]={-1,-1,1,-1,1,1,-1,1};
    float textCoords[8];
    textCoords[0]=0.0f,textCoords[1]=0.0f,textCoords[2]=1.0f,textCoords[3]=0.0f,textCoords[4]=1.0f,textCoords[5]=1.0f,textCoords[6]=0.0f,textCoords[7]=1.0f;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0,(void *)verts);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void *)textCoords);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(activeShaderId,"image"),0);
    glBindTexture(GL_TEXTURE_2D,editableImage->getTextureId());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,(void *)indices);

    //
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    // Graphics::printGlError("1fxShader");

    glUseProgram(globalData->UIProgram);
    //glUniform1i(3,0);
    glViewport(0,0,Graphics::displayParams.screenWidth,Graphics::displayParams.screenHeight);
    // Graphics::printGlError("2fxShader");


    param+=5;
    if(param>360.0)
        param=0.00;
}
void Editor::setActiveSubOption(uint ActiveSubOption)
{
    this->subOptionActive=ActiveSubOption;
}
void Editor::setActiveOption(uint ActiveOption)
{
    this->optionActive=ActiveOption;
    activeShaderId=ShaderManager::createShaderProgram(optionActive);
    if(activeShaderId==0)
    {
        Loge("ERROR:EditingonOptionSet","could not create shader program");
    }
}
void Editor::setOptions(ImageViewStack *optionsMenu, ImageViewStack *subOptionsMenu)
{

}
GLuint ShaderManager::createShaderProgram(uint option)
{//////delete previous unused program;also try removing this class and and in editing context create a method for converting option to shader paths and create shader program using Graphics;
std::string vertexSource=shadersFolder +"/" + "vertex.glsl";
std::string fragmentSource=shadersFolder + "/";
    switch (option)
    {
        case 0:
        {
            fragmentSource+="hsi.glsl";
        }
        break;
        case 1:
        {
            fragmentSource=shadersFolder + "test";
        }
        break;
        default:
        {Loge("ShaderMangage:createShaderProgram","Invalid Option for creating Shader");}

    }
  //  Loge("the shaderloca ","%s ,%s",vertexSource.c_str(),fragmentSource.c_str());
    return Shader::createShaderProgram(AppContext::getApp(),vertexSource.c_str(),fragmentSource.c_str());
}
