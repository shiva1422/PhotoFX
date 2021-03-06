package com.kalasoft.photofx;

import android.opengl.GLES31;
import android.util.Log;

public class Graphics
{
    static public void checkGLError(String tag) {
        int error;
        if ((error = GLES31.glGetError()) != GLES31.GL_NO_ERROR) {
            Log.e("RenderErrorglCheck", tag + ": glError " + error);
        }
    }
}
