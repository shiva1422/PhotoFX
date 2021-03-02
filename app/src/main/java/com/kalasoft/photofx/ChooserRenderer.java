package com.kalasoft.photofx;

import android.opengl.GLES20;
import android.opengl.GLES30;
import android.opengl.GLES31;
import android.opengl.GLSurfaceView;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Arrays;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ChooserRenderer implements GLSurfaceView.Renderer {
    public int programId=0;
    Triangle triangle;
    static float red=0.0f;
    static int test=0;
    static {
        System.loadLibrary("main");
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        programId=createGlProgram("sdf","dfs");//check if pogramcreated;
        GLES31.glUseProgram(programId);
        triangle=new Triangle();
        triangle.mProgram=programId;
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES31.glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
       // Log.e("RendererChooser:draw","the prog id" + programId);
        GLES31.glViewport(0, 0, 200, 200);
        GLES31.glUseProgram(programId);
        red+=0.05f;if(red>1){red=0;}
       // Log.e("PROGRAMID","" +programId);
        GLES31.glClearColor(0.4f, 0.0f, 0.0f, 1.0f);
        GLES31.glClear(GLES20.GL_COLOR_BUFFER_BIT);
       // checkGLError("ChooserRenderre");
        triangle.draw();




    }
    public void checkGLError(String tag) {
        int error;
        if ((error = GLES31.glGetError()) != GLES31.GL_NO_ERROR) {
            Log.e("RenderErrorglCheck", tag + ": glError " + error);
        }
    }
    public native int createGlProgram(String vertexShaderPath,String fragmentShaderPath);
}
