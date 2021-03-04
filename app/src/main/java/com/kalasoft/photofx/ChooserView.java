package com.kalasoft.photofx;

import android.app.Activity;
import android.content.Context;
import android.database.Cursor;
import android.opengl.GLSurfaceView;
import android.provider.MediaStore;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.PopupWindow;

public class ChooserView extends GLSurfaceView
{
    PopupWindow popupWindow;
    public int  uiProgramId;
    Context context;



private ChooserRenderer renderer;
        public ChooserView(Context context)
        {
        super(context);
        this.context=context;
        setEGLContextClientVersion(3);///version check (to 3.0 ma
         super.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);/////depth needed?
         renderer=new ChooserRenderer( context);

         renderer.programId=uiProgramId;
         setRenderer(renderer);
         setOnTouchListener(chooserTouchListener);
                // Render the view only when there is a change in the drawing data
        //setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
        }
        public ChooserView(Context context,int id)
        {
            this(context);
            this.context=context;
            uiProgramId=id;
            renderer.programId=uiProgramId;
        }
    View.OnTouchListener chooserTouchListener=new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if(event.getX()<50&&event.getX()<50)///////take renderer height and width
            {
               popupWindow.dismiss();

            }

            return false;
        }
    };
}
