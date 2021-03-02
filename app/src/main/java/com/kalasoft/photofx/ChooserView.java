package com.kalasoft.photofx;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;

public class ChooserView extends GLSurfaceView
{
    public int  uiProgramId;
private ChooserRenderer renderer;
        public ChooserView(Context context)
        {
        super(context);
        setEGLContextClientVersion(3);///version check (to 3.0 may
            super.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
         renderer=new ChooserRenderer();
         renderer.programId=uiProgramId;
         setRenderer(renderer);
                // Render the view only when there is a change in the drawing data
        //setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        }
        public ChooserView(Context context,int id)
        {
            this(context);
            uiProgramId=id;
            renderer.programId=uiProgramId;
        }
}
