package com.kalasoft.photofx;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES31;
import android.opengl.GLUtils;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.opengles.GL;

import static java.security.AccessController.getContext;

public class Square {
    public int mProgram;
    Context context;

    private FloatBuffer vertexBuffer,textureBuffer;
    private ShortBuffer drawListBuffer;
    static final int COORDS_PER_VERTEX = 3;
    static float squareCoords[] = {
            -0.5f, -0.5f, 0.0f,   // bottom left
            0.5f, -0.5f, 0.0f,   // bottom right
            0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,   // top left
    }; // top right


    private short drawOrder[] = { 0, 1, 2, 0, 2, 3 }; // order to draw vertices

    //texture
    Bitmap image;
    int[] textures = new int[1];
    //textCoords[0]=0.0f,textCoords[1]=0.0f,textCoords[2]=1.0f,textCoords[3]=0.0f,textCoords[4]=1.0f,textCoords[5]=1.0f,textCoords[6]=0.0f,textCoords[7]=1.0f;


    float textureCoordinates[] = {0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f };
    public Square() {
        // initialize vertex byte buffer for shape coordinates
        ByteBuffer bb = ByteBuffer.allocateDirect(squareCoords.length * 4);
        bb.order(ByteOrder.nativeOrder());
        vertexBuffer = bb.asFloatBuffer();
        vertexBuffer.put(squareCoords);
        vertexBuffer.position(0);

        // initialize byte buffer for the draw list
        ByteBuffer dlb = ByteBuffer.allocateDirect(drawOrder.length * 2);
        dlb.order(ByteOrder.nativeOrder());
        drawListBuffer = dlb.asShortBuffer();
        drawListBuffer.put(drawOrder);
        drawListBuffer.position(0);
        GLES31.glGenTextures(1,textures,0);////////delete textureslater;
        GLES31.glBindTexture(GLES31.GL_TEXTURE_2D,textures[0]);
        GLES31.glTexParameterf(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_MAG_FILTER, GLES31.GL_LINEAR);
        GLES31.glTexParameterf(GLES31.GL_TEXTURE_2D, GLES31.GL_TEXTURE_MIN_FILTER, GLES31.GL_LINEAR);
       // image= BitmapFactory.decodeResource(context.getResources(),R.drawable.boom);



    }
    public  void genTExt()
    {
        if(image==null) Log.e("Image","null");
        GLUtils.texImage2D(GLES31.GL_TEXTURE_2D, 0, image, 0);
        textureBuffer = ByteBuffer.allocateDirect(textureCoordinates.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        textureBuffer.put(textureCoordinates);
        textureBuffer.position(0);
    }
    public void draw()
    {   int mPositionHandle;
        mPositionHandle = GLES31.glGetAttribLocation(mProgram, "vPosition");
        int texHandle=GLES31.glGetAttribLocation(mProgram,"texCoords");
        GLES31.glEnableVertexAttribArray(mPositionHandle);
        GLES31.glEnableVertexAttribArray(texHandle);
        GLES31.glVertexAttribPointer(mPositionHandle,COORDS_PER_VERTEX,GLES31.GL_FLOAT,false,0,vertexBuffer);
        GLES31.glVertexAttribPointer(texHandle,2, GLES31.GL_FLOAT,false,0,textureBuffer);
      //  GLES31.glEnable(GLES31.GL_TEXTURE_2D);
        GLES31.glBindTexture(GLES31.GL_TEXTURE_2D,textures[0]);
        GLES31.glDrawElements(GLES31.GL_TRIANGLES,6,GLES31.GL_UNSIGNED_SHORT,drawListBuffer);
    }
}
