package com.kalasoft.photofx;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLES30;
import android.opengl.GLES31;
import android.opengl.GLUtils;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

public class Image
{
    //properties for retrieving image;
    long imageId;
    String imageTitle;
    Bitmap bitmap;
    //AppSpecific
    static Context context;
    //drawing
    int[] texIds=new int[1];
    static int mProgram;//shaderProg
    static final int COORDS_PER_VERTEX = 2;
    static FloatBuffer vertexBuffer,textureBuffer;//indexed drawing only one copy enough;
    static ShortBuffer drawListBuffer;
    private short drawOrder[] = { 0, 1, 2, 0, 2, 3 };
    float textureCoordinates[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,0.0f, 1.0f };
    static float squareCoords[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f,  0.5f, -0.5f,  0.5f};
    Image()
    {
        ByteBuffer bb = ByteBuffer.allocateDirect(squareCoords.length * 4);//////no need square coords or us
        bb.order(ByteOrder.nativeOrder());
        vertexBuffer = bb.asFloatBuffer();
        vertexBuffer.put(squareCoords);
        vertexBuffer.position(0);
        //
        ByteBuffer dlb = ByteBuffer.allocateDirect(drawOrder.length * 2);
        dlb.order(ByteOrder.nativeOrder());
        drawListBuffer = dlb.asShortBuffer();
        drawListBuffer.put(drawOrder);
        drawListBuffer.position(0);
        //
        textureBuffer = ByteBuffer.allocateDirect(textureCoordinates.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        textureBuffer.put(textureCoordinates);
        textureBuffer.position(0);
        //
        GLES31.glGenTextures(1,texIds,0);////////delete textureslater;
        GLES31.glBindTexture(GLES31.GL_TEXTURE_2D,texIds[0]);
        GLES31.glTexParameterf(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_MAG_FILTER, GLES31.GL_LINEAR);
        GLES31.glTexParameterf(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_MIN_FILTER, GLES31.GL_LINEAR);

    }
    void setBounds(int startX,int startY,int width,int height)
    {
        ///X's
        float[] vertices=new float[8];
        vertices[0] =(float) (-1.0 + (startX * 2.0) /ChooserRenderer.width);//*2 so that-1 to 1 else we get 0 to 1 after conversion  leftX
        vertices[2] = (float)(-1.0 + ((startX + width) * 2.0) / ChooserRenderer.width);//rightX
        vertices[4] = vertices[2];
        vertices[6] = vertices[0];
        ///Y's
        vertices[1] = (float)(1.0 - ((startY + height) * 2.0) / ChooserRenderer.height);//bottomy
        vertices[3] = vertices[1];
        vertices[5] = (float)(1.0 - ((startY) * 2.0) / (float) ChooserRenderer.height);//topy
        vertices[7] = vertices[5];
        vertexBuffer.put(vertices);
        vertexBuffer.position(0);

    }

    public void setTexImage()
    {
        GLES31.glBindTexture(GLES31.GL_TEXTURE_2D,texIds[0]);
        if(bitmap==null) {Log.e("Image:setTexImage","null Image");}
        GLUtils.texImage2D(GLES31.GL_TEXTURE_2D, 0, bitmap, 0);

    }
    public void resetTexImage(){
        GLES31.glBindTexture(GLES31.GL_TEXTURE_2D,texIds[0]);
        if(bitmap==null) {Log.e("Image:setTexImage","null Image");}
        GLUtils.texImage2D(GLES31.GL_TEXTURE_2D, 0, bitmap, 0);

    }

    public void draw()
    {
        int posLoc,texCoordLoc;
        posLoc= GLES31.glGetAttribLocation(mProgram, "position");
        texCoordLoc=GLES31.glGetAttribLocation(mProgram,"texCoords");
        GLES31.glEnableVertexAttribArray(posLoc);
        GLES31.glEnableVertexAttribArray(texCoordLoc);
        GLES31.glVertexAttribPointer(posLoc,COORDS_PER_VERTEX,GLES31.GL_FLOAT,false,0,vertexBuffer);
        GLES31.glVertexAttribPointer(texCoordLoc,2, GLES31.GL_FLOAT,false,0,textureBuffer);
        GLES31.glBindTexture(GLES30.GL_TEXTURE_2D,texIds[0]);
        int displayParamsLoc=GLES31.glGetAttribLocation(mProgram,"displayParams");
        GLES31.glUniform2f(displayParamsLoc,ChooserRenderer.width,ChooserRenderer.height);
        GLES31.glDrawElementsInstanced(GLES30.GL_TRIANGLES,6,GLES31.GL_UNSIGNED_SHORT,drawListBuffer,15);


    }





}
