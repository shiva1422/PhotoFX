//
// Created by Shiva Shankar patel psy on 11/30/20.
//

#include "UI.h"
#include "UIShaderConstants.h"
#include "Shapes.h"
#include "JavaCalls.h"
DisplayParams View::displayParams;
Bitmap ImageView::defaultImage;
//float textureCoords[8]={0.0f,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f};
//GLushort drawElementIndices[6]={0,1,2,2,3,0};
GLuint ImageView::texCoodBufId=0,ImageView::indexBufId=0;


void ViewGroup::draw()
{
    for(int i=0;i<noViews;i++)
        if(views[i])
        views[i]->draw();

}
void ViewGroup::removeView(View *view)
{
    for(int i=0;i<noViews;i++)
    {
        if(views[i]==view)
        {
            for(int k=i;k<noViews-1;k++)
            {
                views[k]=views[k+1];
            }
            noViews--;
            Loge("Viewgroup","view removed");
            break;
        }
    }
}
void ViewGroup::addView(View *view)
{
    //Use hashng ids for views;
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
    views=new View*[defaultSize];//(View **)malloc(defaultSize*sizeof(View *));
    setOnTouchListener(new ViewGroupTouchListener());
   // onTouch=(OnTouch)&ViewGroup::defaultOnTouch;
}
ViewGroup::~ViewGroup()
{
    for(int i=0;i<noViews;i++)
    {
        delete views[i];
    }
    delete[] views;
    Loge("ViewGroup","destructed");
}

void ImageViewStack::draw()
{
    //View::draw();
    glUniform1i(DRAWTYPELOC,IMAGEVIEWSTACKDRAWTYPE);
    float gapX=(viewGap*2.0)/displayParams.screenWidth;//////different for horizontal and vertical displayparams.screenHeight
    float imageWidth=singleImageWidth*2.0/displayParams.screenWidth;////height for vertical
    glUniform1f(PARAMSLOC, gapX);
    glUniform1f(PARAM2LOC,imageWidth);
    glUniform1i(PARAM3LOC,(int)activeViewNo);
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
  ///  Graphics::printGlError("ImageViewStack::draw");

}
int ImageViewStack::getViewNoAtLoc(float x, float y)////improve as this returns out of numViews also gaps also account for(should not be)
{//use x for horizontal orientation ,y for vertical orientation///needs some improvesment as afiter fit view to bounds chnages in dimensions;
    float length=x-startX;
    if(!isPointInside(x,y))
    {
        return INT32_MAX;
    }
    else
    {
        float no=(length/(singleImageWidth+viewGap));       //should give accurate improve
      //  Loge("ImageViewStack:GetViewNOLoc","%f",no);
      if(no<numViews)
        return no;
      else return activeViewNo;
    }

}
void ImageViewStack::fitViewsInBounds()
{
    //convert bitmap width & height to dp here;
    singleImageWidth=bitmapWidth,singleImageHeight=bitmapHeight;//mostly these two would be same;
    float aspectRatio=bitmapWidth/bitmapHeight;
    if(bitmapHeight>height)
    {
        singleImageHeight=height;
        singleImageWidth=singleImageHeight*aspectRatio;
    }
    float totalWidthRequired=(numViews+1)*viewGap+numViews*singleImageWidth;
    if(totalWidthRequired>width)
    {
         singleImageWidth=(width-(numViews+1)*viewGap)/numViews;
         singleImageHeight=singleImageWidth/aspectRatio;

    }
    else if(totalWidthRequired<width)
    {
        viewGap=(width-numViews*singleImageWidth)/(numViews+1);
    }
    setSingleViewBounds();
}
void ImageViewStack::setSingleViewBounds()
{
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    if(numViews!=0)
    {
        float *vertices = (float *) glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * 8,GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
        if (vertices)//vertices of first View;
        {
            float tempStartX=startX,tempyStartY=startY;
            if(singleImageHeight<height)
            {
                tempyStartY = startY + (height - singleImageHeight) / 2.0;
            }
            ///X's
            vertices[0] = -1.0 + (tempStartX* 2.0) /(float) displayParams.screenWidth;//*2 so that-1 to 1 else we get 0 to 1 after conversion  leftX
            vertices[2] = -1.0 + ((tempStartX + singleImageWidth) * 2.0) /(float) displayParams.screenWidth;//rightX
            vertices[4] = vertices[2];
            vertices[6] = vertices[0];
            ///Y's
            vertices[1] =1.0 - ((tempyStartY+ singleImageHeight) * 2.0) / (float) displayParams.screenHeight;//bottomy//single ImageHeight;
            vertices[3] = vertices[1];//topy
            vertices[5] = 1.0 - ((tempyStartY) * 2.0) / (float) displayParams.screenHeight;
            vertices[7] = vertices[5];
        } else {////lazy draw on
            ///uploading vertices everydrawcall
            //UILogE("ImageView::setBouds-error uploading vertices");
            // Graphics::printGlError("ImageView::setBouds()");
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
void ImageViewStack::fillTexture(int viewNo, Bitmap *bitmap)
{
    glBindTexture(GL_TEXTURE_2D_ARRAY,texId);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,viewNo,bitmapWidth,bitmapHeight,1,GL_RGBA,GL_UNSIGNED_BYTE,bitmap->pixels);//bitmapWIdth,height should be set;
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    if(Graphics::printGlError("ImageViewSTACKE::fillTexture")==GL_NO_ERROR) {
        //  UILogE("succesfully created the image stack");
    }


}
void ImageViewStack::setTextureForViewNo(int viewNo,const char *assetLoc)
{
    JavaCalls::setImageViewStackTexture(this,viewNo,assetLoc);//check return for errs
}
void ImageViewStack::setNoViews(int numViews, int32_t imageWidth, int32_t imageHeight)
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
    glTexStorage3D(GL_TEXTURE_2D_ARRAY,1,GL_RGBA8,imageWidth,imageHeight,numViews);///errorcheccing////if this function is public the previous texture array should be deleted;
  /*  for(int i=0;i<numViews;i++)//not filling default image
    {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,0,0,i,imageWidth,imageHeight,1,GL_RGBA,GL_UNSIGNED_BYTE,ImageView::defaultImage.pixels);
        ///MAKE LAST PARAMETER IN ABOUve FUNC TO NULL FOR  speedup initializeing(upload image when required).

    }*/
    glBindTexture(GL_TEXTURE_2D_ARRAY,0);
    if(Graphics::printGlError("ImageViewSTACKE::SETNOVIEWS")==GL_NO_ERROR) {
        //  UILogE("succesfully created the image stack");
    }
    bitmapHeight=imageHeight;
    bitmapWidth=imageWidth;

}
ImageViewStack::ImageViewStack(int numViews, int32_t imageWidth, int32_t imageHeight):ImageViewStack()
{

    setNoViews(numViews,imageWidth,imageHeight);
}
void ImageViewStack::setBounds(float startX, float startY, float width, float height)
{
    View::setBounds(startX,startY,width,height);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    singleImageWidth=(width-(numViews+1)*viewGap)/numViews;
    singleImageHeight=height;
    setSingleViewBounds();
}
ImageViewStack::ImageViewStack()
{
    glGenBuffers(1,&vertexBufId);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);///dimensions should be set before this or else reset dimesnion with same dims
    glBindBuffer(GL_ARRAY_BUFFER,0);
    setOnTouchListener(new ImageViewStackClickListener());///clear previous touch listeners and current

}
void ImageView::draw()
{
    //View::draw();//DrawsBackGroundColor//remove if not needed
    //printVerts();

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
    //if(Graphics::printGlError("ImageView::ImageView(Bounds,Bitmap *)")==GL_OUT_OF_MEMORY)//remvoe comment;
       // return texture;
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
    Loge("ConstructorLast","3");
 setTexture(image);
}
ImageView::ImageView(float startX, float startY, float width, float height):ImageView()
{
    Loge("ConstructorLast","2");
    setBounds(startX,startY,width,height);
}
void ImageView::setTextureId(GLuint texId)
{
    this->texId=texId;/////////////check other thing need to change if changed textureId like clear previous texture;
}
void ImageView::setTexture(const char *assetLoc,bool shouldFitToPreviousBounds)
{
    if(JavaCalls::setImageViewTexture(this,assetLoc)==STATUS_KO)
    {
        UILogE("could not set texture %s",assetLoc);
    }
    float previousWidth=width;
    float previousHeight=height;
    setBoundsDeviceIndependent(this->startX,this->startY,image.width,image.height);
    if(shouldFitToPreviousBounds)
    fitToCentre(startX,startY,previousWidth,previousHeight);
}
void ImageView::setTexture(Bitmap *image)
{
    ////////need to clear the preious texture;
    if(!image->pixels)
    {UILogE("image setTexture","no image pixels");}
    this->image.height=image->height;//if(image!=this->image)
    this->image.width=image->width;
    this->image.stride=image->stride;
    this->image.pixels=image->pixels;///remove reference to image as it might be GC after Jni call;
    if(glIsBuffer(texBufId))
    {glDeleteBuffers(1,&texBufId);}
    glGenBuffers(1, &texBufId);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texBufId);//use right buffer or else slow
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->width * image->height * 4,image->pixels ,GL_STATIC_COPY);
    if(Graphics::printGlError("ImageView::setTex")==GL_OUT_OF_MEMORY)
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
    Graphics::printGlError("ImageView::setTex,glTextStorage");
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glBindTexture(GL_TEXTURE_2D,0);
    if(Graphics::printGlError("ImageView::setTex")==GL_NO_ERROR)
        isTextureSet=true;
   Loge("SetTexture","done");
}
void ImageView::setBounds(ImageView *imageView)
{
    setBounds(imageView->getStartX(),imageView->getStartY(),imageView->width,imageView->height);
}
void ImageView::printVerts()
{
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufId);
    float *vertices=(float *)glMapBufferRange(GL_ARRAY_BUFFER,0,sizeof(float)*8,GL_MAP_READ_BIT);
    if(vertices)
    {
       for(int i=0;i<8;i++)
       {
           Loge("thevertn o","%d",i);
           Loge("ImageVerts","verts[%d]- %f",i,vertices[i]);
       }
    }
    else
    {
        UILogE("ImageView PrintVerts","could not map");
        Graphics::printGlError("ImageView::setBouds()");
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);//return GL_false if error
    glBindBuffer(GL_ARRAY_BUFFER,0);
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
    glBufferData(GL_ARRAY_BUFFER,sizeof(float)*8,(void *)0,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    setTexture(&defaultImage);///remove release;
}
void View::clearRect(float r,float g,float b,float a)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(startX,displayParams.screenHeight-startY-height,width,height);
    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}
void View::clearRect()
{

    glEnable(GL_SCISSOR_TEST);
    glScissor(startX,displayParams.screenHeight-startY-height,width,height);//if glScissor enable after this then scissor bounds should bes same as viewport bounds;
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
void View::setEndY(float endY)
{
    setBounds(startX,endY-height,width,height);
}
void View::setStartY(float startY)
{
    setBounds(startX,startY,width,height);
}
void View::setEndX(float endX)
{
    setBounds(endX-width,startY,width,height);
}
void View::setStartX(float startX)
{
    setBounds(startX,startY,width,height);
}
void View ::moveHorizontalByDistance(float moveDistanceX)
{
    setBounds(startX+moveDistanceX,startY,width,height);
}
void View::setBounds(float startX, float startY, float width, float height)
{
    this->startX=startX;
    this->startY=startY;
    this->width=width;
    this->height=height;
    /* no need as the following is done in subclasses accordingly;
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
kforceinline bool View::isPointInside(float x, float y)
{
    return (x>=startX&&x<=(startX+width)&&y>=startY&&y<=(startY+height));

}
kforceinline bool View::isPointXInside(float x)
{
    return(x>=startX&&x<=(startX+width));
}
kforceinline bool View::isPointYInside(float y)
{
    return (y>=startY&&y<=(startY+height));
}
kforceinline bool View::isPointToTheRight(float x)
{
    return (x>(startX+width));
}
kforceinline bool View::isPointToTheLeft(float x)
{
    return (x<startX);
}
kforceinline bool View::isPointAbove(float y)
{
    return(y<startY);
}
kforceinline bool View::isPointBelow(float y)
{
    return(y>(startY+width));
}
View::~View()
{
    delete onTouchListener;
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
bool ViewGroup::dispatchTouchToChildViews(float touchX, float touchY, int32_t pointerId, TouchAction touchAction)
{
    //use switch case for all options;
    for(int i=0;i<noViews;i++)
    {
        if(views[i]&&views[i]->getVisibility())
        {
            if(views[i]->isTouched(touchX,touchY,pointerId,touchAction))
            {
                return true;
            }
        }
    }
    return false;
}
bool ViewGroup::isTouched(float touchX, float touchY,int pointerId,TouchAction touchAction)
{
   return View::isTouched(touchX,touchY,pointerId,touchAction);

}
bool View::isTouched(float touchX,float touchY,int pointerId,TouchAction touchAction){

 //   UILogE("view touched");
 ////Use switch case for all actions;

    bool touched= touchX >= startX && touchY >= startY && touchX <= (startX + width) &&
                  touchY <= (startY + height);
    if(touched||(pointerId==onTouchListener->getPreviousPointerId())){
      //  onTouchListener->defaultOnTouch(touchX,touchY,ACTION_DOWN);
     return onTouchListener->onTouch (touchX,touchY,pointerId,touchAction,this);
    };
    return false;
};