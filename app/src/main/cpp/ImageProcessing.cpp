//
// Created by Shiva Shankar patel psy on 2/13/21.
//

#include "ImageProcessing.h"
#include "Graphics.h"
#include "Main.h"
android_app *PhotoFX::app= nullptr;
void PhotoFX::apply()
{
    srand(time(nullptr));
    static float param=1.0;

   // Loge("param value","the values is %f ",param);
    glUseProgram(shaderProgram);
    GlobalData *globalData=(GlobalData *)(app->userData);
    glBindFramebuffer(GL_FRAMEBUFFER,globalData->frameBufId);
    glViewport(0,0,inputImage->width,inputImage->height);
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUniform1i(glGetUniformLocation(shaderProgram,"frameBuf"),1);
    glUniform1f(1,param);

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
    glUniform1i(glGetUniformLocation(shaderProgram,"image"),0);
    glBindTexture(GL_TEXTURE_2D,inputTexId);
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
PhotoFX::PhotoFX()
{
    shaderProgram=Shader::createShaderProgram(app,"Filters/vertex.glsl","Filters/hsi.glsl");
}
