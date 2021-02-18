//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#include "UI.h"
DisplayParams View::displayParams;
Bitmap ImageView::defaultImage;
GLint DRAWTYPELOC=0,PARAM1LOC=1,PARAM2LOC=2,PARAM3LOC=4,SAMPLERARRAYLOC=7;
GLint IMAGEVIEWDRAWTYPE=0,IMAGEVIEWSTACKDRAWTYPE=1,SLIDERSETDRAWTYPE=2;
GLint POSITIONATTRIBLOC=0,TEXTCOODATTRIBLOC=2;
//float textureCoords[8]={0.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f};
//GLushort drawElementIndices[6]={0,1,2,2,3,0};
GLuint ImageView::texCoodBufId=0,ImageView::indexBufId=0;

void ViewGroup::draw()
{
    for(int i=0;i<noViews;i++)
        views[i]->draw();

}
void ViewGroup::addView(View *view)
{
    ////initialize ViewGroup with predefined size and use malloc if exeded that size instead of doing for every add;
    ///Check malloc error handle accordingly
   if(noViews<defaultSize)
   {
       views[noViews]=view;
   }
   noViews++;

   /* View **tempViews=nullptr;
    tempViews=(View **)malloc(noViews *sizeof(View **));//first row or viewlist pointer
    for(int i=0;i<noViews;i++)
    {
        *(tempViews+i) = *(views+i);
    }

    //
    noViews++;
    if(views)
    {
        free(views);
        *views=(View *)malloc(noViews*sizeof(View *));
        for(int i=0;i<(noViews-1);i++)
        {
           *(views+i)=*(tempViews+i);
        }
        *(views+(noViews-1))=*(tempViews+noViews-1);
    }
    free(tempViews);*/

}
ViewGroup::ViewGroup()
{//should be edited completey just for times sake use new() if pos also use references instead of pointers.
    views=(View **)malloc(defaultSize*sizeof(View *));
   // onTouch=(OnTouch)&ViewGroup::defaultOnTouch;
}
void SliderSet::draw()
{
  //  View::draw();
    baseImageView.clearRect();
    pointerImageView.clearRect();//.draw() for images


}
void SliderSet::setTexture(Bitmap *image)
{
   /* this->baseImage=image;
    if(glIsBuffer(texBufId))
    {glDeleteBuffers(1,&texBufId);}
    glGenBuffers(1, &texBufId);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBufId);//use right buffer or else slow
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->width * image->height * 4,image->pixels ,GL_STATIC_COPY);
    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *)")==GL_OUT_OF_MEMORY)
        return;
    if(glIsTexture(texId))
        glDeleteTextures(1,&texId);
    glGenTextures(1, &texId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image->width,image->height);//wiki commonmistakes//use glTexImage for mutable textures.//glpixelstore for way to read(pack)and write(unpack) image using this fun.
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,image->width,image->height,GL_RGBA,GL_UNSIGNED_BYTE,0);
    Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *),glTextStorage");
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap*)")==GL_NO_ERROR)
        isTextureSet=true;*/
}
void SliderSet::setPointerLoc(float x, float y)
{
    pointerImageView.setBounds(x,pointerImageView.getStartY(),pointerImageView.getWidth(),pointerImageView.getHeight());//only x changes for horizontal orientation y for verticals
    value=(x-startX)/width;
    UILogE("the slider value is %f",value);
}
void SliderSet::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    baseImageView.setBounds(startX,startY+height/3,width,height/3);
    pointerImageView.setBounds(startX+(width/2),startY,50,height);

}
SliderSet::SliderSet()
{

    baseImageView.setTexture(&ImageView::defaultImage);
    pointerImageView.setTexture(&ImageView::defaultImage);
    onTouchListener=new SliderTouchListener();//clear previous



}

void ImageViewStack::draw()
{
    //View::draw();
    glUniform1i(DRAWTYPELOC,IMAGEVIEWSTACKDRAWTYPE);
    float gapX=(viewGap*2.0)/displayParams.screenWidth;//////different for horizontal and vertical displayparams.screenHeight
    float imageWidth=singleImageWidth*2.0/displayParams.screenWidth;////height for vertical
    glUniform1f(PARAM1LOC,gapX);
    glUniform1f(PARAM2LOC,imageWidth);
    glUniform1i(PARAM3LOC,(int)activeView);
    glEnableVertexAttribArray(POSITIONATTRIBLOC);
    glEnableVertexAttribArray(TEXTCOODATTRIBLOC);
    //draw texture rect
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glVertexAttribPointer(POSITIONATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 0,(void *)0);
    glBindBuffer(GL_ARRAY_BUFFER,ImageView::texCoodBufId);
    glVertexAttribPointer(TEXTCOODATTRIBLOC,2,GL_FLOAT,GL_FALSE,0,(void *)0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY,texId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ImageView::indexBufId);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,(void *)0,numViewsToDraw);
    //
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
   // Graphics::printGlError("ImageViewStack::draw");

}
uint ImageViewStack::getViewNoAtLoc(float x, float y)
{//use x for horizontal orientation ,y for vertical orientation
    float length=x-startX;
    if(x<0.0f)
    {
        return UINT_MAX;
    }
    else
    {
        float no=(length/(singleImageWidth+viewGap));
      //  Loge("ImageViewStack:GetViewNOLoc","%f",no);
        return (uint)no;
    }

}
void ImageViewStack::setNoViews(uint numViews, int32_t imageWidth, int32_t imageHeight)
{
    this->numViews=numViews;
    this->numViewsToDraw=numViews;
    this->startViewNo=0;
    glGenTextures(1,&texId);//texId is the textureArrayId
    glUniform1i(SAMPLERARRAYLOC,1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY,texId);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY,1,GL_RGBA8,imageWidth,imageHeight,numViews);
    for(int i=0;i<numViews;i++)
    {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,i,imageWidth,imageHeight,1,GL_RGBA,GL_UNSIGNED_BYTE,ImageView::defaultImage.pixels);
        ///MAKE LAST PARAMETER IN ABOUve FUNC TO NULL FOR  speedup initializeing(upload image when required).

    }
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    if(Graphics::printGlError("ImageViewSTACKE::SETNOVIEWS")==GL_NO_ERROR)
        UILogE("succesfully created the image stack");

}
ImageViewStack::ImageViewStack(uint numViews, int32_t imageWidth, int32_t imageHeight):ImageViewStack()
{

    setNoViews(numViews,imageWidth,imageHeight);
}
void ImageViewStack::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    if(numViews!=0)
    {
        float *vertices = (float *) glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * 8,GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
        if (vertices)//vertices of first View;
        {
            ///X's
            vertices[0] = -1.0 + (startX * 2.0) /(float) displayParams.screenWidth;//*2 so that-1 to 1 else we get 0 to 1 after conversion  leftX
            singleImageWidth=(width-(numViews+1)*viewGap)/numViews;
            vertices[2] = -1.0 + ((startX + singleImageWidth) * 2.0) /(float) displayParams.screenWidth;//rightX
            vertices[4] = vertices[2];
            vertices[6] = vertices[0];
            ///Y's
            vertices[1] =
                    1.0 - ((startY + height) * 2.0) / (float) displayParams.screenHeight;//bottomy
            vertices[3] = vertices[1];//topy
            vertices[5] = 1.0 - ((startY) * 2.0) / (float) displayParams.screenHeight;
            vertices[7] = vertices[5];
        } else {////lazy draw on
            ///uploading vertices everydrawcall
            UILogE("ImageView::setBouds-error uploading vertices");
            Graphics::printGlError("ImageView::setBouds()");
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
ImageViewStack::ImageViewStack()
{
    glGenBuffers(1,&vertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);///dimensions should be set before this or else reset dimesnion with same dims
    glBindBuffer(GL_ARRAY_BUFFER,0);
    onTouchListener=new ImageViewStackTouchListener();///clear previous touch listeners and current

}
void ImageView::draw()
{
    //View::draw();//DrawsBackGroundColor//remove if not needed

    glUniform1i(DRAWTYPELOC,IMAGEVIEWDRAWTYPE);

    glEnableVertexAttribArray(POSITIONATTRIBLOC);
    glEnableVertexAttribArray(TEXTCOODATTRIBLOC);

    //draw texture rect
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glVertexAttribPointer(POSITIONATTRIBLOC, 2, GL_FLOAT, GL_FALSE, 0,(void *)0);
    glBindBuffer(GL_ARRAY_BUFFER,texCoodBufId);
    glVertexAttribPointer(TEXTCOODATTRIBLOC,2,GL_FLOAT,GL_FALSE,0,(void *)0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBufId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,(void *)0);

    //
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
}
Texture ImageView::createTexture(Bitmap *image)
{
    Texture texture;
    glGenBuffers(1, &texture.texBufId);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture.texBufId);//use right buffer or else slow
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->width * image->height * 4,image->pixels ,GL_STATIC_COPY);
    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *)")==GL_OUT_OF_MEMORY)
        return texture;
    glGenTextures(1, &texture.texId);
    glBindTexture(GL_TEXTURE_2D, texture.texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image->width,image->height);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,image->width,image->height,GL_RGBA,GL_UNSIGNED_BYTE,0);
    Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *),glTextStorage");
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
    //if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap*)")==GL_NO_ERROR);

    return texture;
}
////CONSIDER ACTIVE TEXTURE UNITs
ImageView::ImageView(float startX, float startY, float width, float height, Bitmap *image):ImageView( startX,  startY,  width,  height)
{

 setTexture(image);
}
ImageView::ImageView(float startX, float startY, float width, float height):ImageView()
{
    setBounds(startX,startY,width,height);
}
void ImageView::setTextureId(GLuint texId)
{
    this->texId=texId;/////////////check other thing need to change if changed textureId like clear previous texture;
}

void ImageView::setTexture(Bitmap *image)
{
    ////////need to clear the preious texture;
    this->image=image;//if(image!=this->image)cheeck
    if(glIsBuffer(texBufId))
    {glDeleteBuffers(1,&texBufId);}
    glGenBuffers(1, &texBufId);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBufId);//use right buffer or else slow
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->width * image->height * 4,image->pixels ,GL_STATIC_COPY);
    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *)")==GL_OUT_OF_MEMORY)
        return;
    if(glIsTexture(texId))
        glDeleteTextures(1,&texId);
    glGenTextures(1, &texId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image->width,image->height);//wiki commonmistakes//use glTexImage for mutable textures.//glpixelstore for way to read(pack)and write(unpack) image using this fun.
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,image->width,image->height,GL_RGBA,GL_UNSIGNED_BYTE,0);
    Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *),glTextStorage");
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
    if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap*)")==GL_NO_ERROR)
        isTextureSet=true;
}
void ImageView::setBounds(ImageView *imageView)
{
    setBounds(imageView->getStartX(),imageView->getStartY(),imageView->width,imageView->height);
}
void ImageView::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    float *vertices=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*8,GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    if(vertices)
    {
        ///X's
        vertices[0] = -1.0 + (startX * 2.0) /(float) displayParams.screenWidth;//*2 so that-1 to 1 else we get 0 to 1 after conversion  leftX
        vertices[2] = -1.0 + ((startX + width) * 2.0) / (float) displayParams.screenWidth;//rightX
        vertices[4] = vertices[2];
        vertices[6] = vertices[0];
      ///Y's
        vertices[1] = 1.0 - ((startY + height) * 2.0) / (float) displayParams.screenHeight;//bottomy
        vertices[3] = vertices[1];
        vertices[5] = 1.0 - ((startY) * 2.0) / (float) displayParams.screenHeight;//topy
        vertices[7] = vertices[5];
    }
    else
    {////lazy draw on
        ///uploading vertices everydrawcall
        UILogE("ImageView::setBouds-error uploading vertices");
        Graphics::printGlError("ImageView::setBouds()");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    glBindBuffer(GL_ARRAY_BUFFER,0);


}
ImageView::ImageView()
{
    glGenBuffers(1,&vertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);///dimensions should be set before this or else reset dimesnion with same dims
    glBindBuffer(GL_ARRAY_BUFFER,0);
    setTexture(&defaultImage);
}
void View::clearRect()
{

    glEnable(GL_SCISSOR_TEST);
    glScissor(startX,displayParams.screenHeight-startY-height,width,height);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
void View::fitToCentre(const View &view)
{
    fitToCentre(view.startX,view.startY,view.width,view.height);
}
void View::fitToCentre(float borderStartX, float borderStartY, float borderWidth, float borderHeight)
{
    float newWidth,newHeight;
    //float aspectRatioImage=width/height;
    float xRation,yRation;
    xRation=width/borderWidth;
    yRation=height/borderHeight;
    if(xRation>1||yRation>1)
    {
        float finalRatio;
        if(xRation>=yRation)
            finalRatio=xRation;
        else
            finalRatio=yRation;

        newHeight=height/finalRatio;
        newWidth=width/finalRatio;

    }
    else
    {
        newWidth=width;
        newHeight=height;
    }
    float  xdiff=borderWidth-newWidth;
    float  ydiff=borderHeight-newHeight;
  //  {UILogE("negative difference xdiff %f and ydiff %f",xdiff,ydiff);}
    setBounds(borderStartX+(xdiff/2),borderStartY+(ydiff/2),newWidth,newHeight);
}

void View:: setBoundsDeviceIndependent(float xStart,float yStart,float width, float height)
{
    //consider screenDensity in X and Y direction to get Density indpendent pixel sizes;
    //px = dp * (dpi / 160)
    //consider the image dimensions as dp and return in px dims for displaying.
    //  UILOG("xdpi %f and ydpi %f",displayParams.xdpi,displayParams.ydpi);
    float newWidth=(width*displayParams.xdpi/displayParams.densityDpi);
    float newHeight=(height*displayParams.ydpi/displayParams.densityDpi);
    setBounds(xStart,yStart,newWidth,newHeight);

}
void View::setBounds(float startX, float startY, float width, float height)
{
    this->startX=startX;
    this->startY=startY;
    this->width=width;
    this->height=height;
    ///X's ///////below can be removed along with vertices as they are directly store on gpuside..buffer.
    //   if(startX<=screenWidth/2)
    vertices[0] =-1.0+ (startX * 2.0) /(float)displayParams.screenWidth;//*2 so that-1 to 1 else we get 0 to 1 after conversion  leftX
    vertices[2] =-1.0+((startX+width)*2.0)/(float)displayParams.screenWidth;//rightX
    vertices[4] =vertices[2];
    vertices[6] =vertices[0];
///Y's
    vertices[1]=1.0-((startY+height)*2.0)/(float)displayParams.screenHeight;//bottomy
    vertices[3]=vertices[1];
    vertices[5]=1.0-((startY)*2.0)/(float)displayParams.screenHeight;
    vertices[7]=vertices[5];
    /*  for(int i=0;i<8;i++)
      {
          UILOG("the vertex %d is %f",i,vertices[i]);
      }*/

}
View::View()
{

}
void InitializeUI()
{

    GLuint bufferIds[2];
    glGenBuffers(2,bufferIds);
    ImageView::indexBufId=bufferIds[0],ImageView::texCoodBufId=bufferIds[1];
    //for(int i=0;i<3;i++)UILogE("%d, vertexBufId %d",bufferIds[i],vertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER,ImageView::texCoodBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);
    float *textCoords=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*8,GL_MAP_WRITE_BIT);
    if(textCoords)
    {

            //0.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f
            textCoords[0]=0.0f,textCoords[1]=0.0f,textCoords[2]=1.0f,textCoords[3]=0.0f,textCoords[4]=1.0f,textCoords[5]=1.0f,textCoords[6]=0.0f,textCoords[7]=1.0f;

    }
    else
    {////lazy draw on
        ///uploading vertices everydrawcall
        UILogE("UIINITIalize()-error uploading textCoods");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ImageView::indexBufId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short )*6, (void *)0, GL_STATIC_DRAW);
    GLushort *indices=(GLushort *)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,0,sizeof(short )*6,GL_MAP_WRITE_BIT);
    if(indices)
    {
        indices[0]=0, indices[1]=1, indices[2]=2, indices[3]=2, indices[4]=3, indices[5]=0;

    }
    else
    {////lazy draw on
        ///uploading vertices everydrawcall
        UILogE("UIINITIALIZe()-error uploading indices");
    }
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}

/*function may need to be put back
 *
 *void ImageView::setupBuffers()
{

    glGenBuffers(1,&vertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);///dimensions should be set before this or else reset dimesnion with same dims
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
 */


////Touch Methods for all Views below only
bool ViewGroup::isTouched(float touchX, float touchY,int pointerId,TouchAction touchAction)
{
    //  (View::*onTouch)(x,y,ACTION_DOWN);
    if(touchX >= startX && touchY >= startY && touchX <= (startX + width) &&touchY <= (startY + height))
    {
        // (this->*onTouch)(touchX,touchY,ACTION_DOWN);
     //   UILogE("ViewGroup touched");
        for(int i=0;i<noViews;i++)
        {
            if(views[i]->isTouched(touchX,touchY,pointerId,touchAction))
            {
                break;
               // onTouchListener->defaultOnTouch(touchX,touchY,ACTION_DOWN);
            }
        }
       // onTouchListener->defaultOnTouch(touchX,touchY,ACTION_DOWN);
        return true;

    }
    return false;

}
bool View::isTouched(float touchX,float touchY,int pointerId,TouchAction touchAction){

 //   UILogE("view touched");

    bool touched= touchX >= startX && touchY >= startY && touchX <= (startX + width) &&
                  touchY <= (startY + height);
    if(touched||(pointerId==onTouchListener->getPreviousPointerId())){
      //  onTouchListener->defaultOnTouch(touchX,touchY,ACTION_DOWN);
      onTouchListener->onTouch (touchX,touchY,pointerId,touchAction,this);
    };
    return touched;
};