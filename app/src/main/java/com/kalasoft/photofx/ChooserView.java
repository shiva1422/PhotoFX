package com.kalasoft.photofx;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class ChooserView extends GLSurfaceView
{
private ChooserRenderer renderer;
        public ChooserView(Context context)
        {
        super(context);
        setEGLContextClientVersion(2);///version check (to 3.0 may
         renderer=new ChooserRenderer();
         setRenderer(renderer);
                // Render the view only when there is a change in the drawing data
        //setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);


        }
}
