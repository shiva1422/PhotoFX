package com.kalasoft.photofx;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.opengl.GLES20;
import android.opengl.GLES30;
import android.opengl.GLES31;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.provider.MediaStore;
import android.util.Log;
import android.util.Size;
import android.view.MotionEvent;
import android.view.VelocityTracker;
import android.view.View;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ChooserRenderer implements GLSurfaceView.Renderer {/////////Opengl updates should be done on the thread only for example onTouch caled from ChooserView so can do image uploads therer
    static int width, height,startX=0,startY=0;
    public int programId=0;
   // Image testImage;
    Image[] images;

    private int totalImagesToDraw,startImageNo=0;//to display
    static float red=0.0f;
    static {
        System.loadLibrary("main"); }
    Context context;

    //Media Cursor
    private boolean imageUpdateNeeded=true,rowupNeed=false,rowDownNeed=false;
    private Cursor mediaCursor;
    private int cursorPosFirstImg=0,cursorPosLastImage=0;
    //projection=table columns/////some are api dependant check;
    private String[] projection=new String[]{ MediaStore.Images.Media._ID,
            MediaStore.Images.Media.DATA,
            MediaStore.Images.Media.DATE_ADDED,
            MediaStore.Images.Media.MIME_TYPE, MediaStore.Images.Media.TITLE,//MediaStore.Images.Media.OWNER_PACKAGE_NAME,
    //MediaStore.Images.Media.RELATIVE_PATH
          //  MediaStore.Images.Media.RELATIVE_PATH//if realtive path is null check MediaStore.MediaColumns api dependent also check data and displaname
    };///instead of Files use Images & vicecv directly
    private String[] selectionArgs;
    private String selection;//= MediaStore.Files.FileColumns.MEDIA_TYPE + "=" + MediaStore.Files.FileColumns.MEDIA_TYPE_IMAGE ;
    private String sortOrder =MediaStore.Images.Media.DATE_ADDED + " DESC";

    ///Drawing
    private float maxImageHeight,maxImageWidth;
    private int totalActiveView=0,numRows=0,numColoumns=3;
    private float gap=5.0f;//gap in pixels


    //Touch
    private int pointerId,decelerationTimeInFrames=100;
    private boolean velocitySignPositive=false,shouldDecelerate=true,isScrolling=false;
    private float yTouchVelocity=0.0f,deceleration=0;
   private float initialTouchX=0.0f,initialTouchY=0.0f,previousTouchX=0.0f,previousTouchY=0.0f,totalMoveDisY=0.0f,moveDisForVelocity=0.0f;
   private VelocityTracker touchVelocityTracker=null;


ChooserRenderer(Context context) {
    this.context = context;
        //no need as we get width and height on surface created;
        // WindowManager windowManager=(WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
        //windowManager.getDefaultDisplay().getRealMetrics(displayParams);//api dependent //windowManager.getCurrentWindowMetric


}
public native int createGlProgram(String vertexShaderSource,String fragmentShaderSource);

    @Override
    public void onDrawFrame(GL10 gl) {
      // Log.e("RendererChooser:draw","the prog id" + programId);
        if(yTouchVelocity!=0)///move to sepeate function inside if
        {
            moveDisForVelocity=-1*yTouchVelocity*(float)0.1;
            onMove(moveDisForVelocity);
            Log.e("move based " ,"in draw is " +moveDisForVelocity);
            if(shouldDecelerate)
            {
                yTouchVelocity-=deceleration;
            }
            if(velocitySignPositive&&yTouchVelocity<0)//jus to track when the velocity changes from -ve  to +ve so to stop moving based on velocity;
            {
                yTouchVelocity=0;
                moveDisForVelocity=0.0f;
                velocitySignPositive=true;
            }
            else if(!velocitySignPositive && yTouchVelocity>0)
            {
                yTouchVelocity=0;
                moveDisForVelocity=0.0f;
                velocitySignPositive=true;
            }
        }
        GLES31.glUseProgram(programId);
        red+=0.05f;if(red>1){red=0;}
       // Log.e("PROGRAMID","" +programId);
        GLES31.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        GLES31.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        Graphics.checkGLError("ChooserRenderre");
        int displayParamsLoc=GLES31.glGetUniformLocation(programId,"displayParams");
        int imageNumLoc=GLES31.glGetUniformLocation(programId,"imageNum");
        int matrixSizeLoc=GLES31.glGetUniformLocation(programId,"numCols");
        int gapLoc=GLES31.glGetUniformLocation(programId,"gap");
        GLES31.glUniform1f(gapLoc,gap);
        GLES31.glUniform1i(matrixSizeLoc,numColoumns);
        GLES31.glUniform2f(displayParamsLoc,width,height);
        if(imageUpdateNeeded)
        {
            calcuateImageTextureAndBound();
            imageUpdateNeeded=false;
           // rowUp();
        }
        if(rowupNeed)
        {
            retrieveOnRowUp();
            rowupNeed=false;
        }
        if(rowDownNeed)
        {
            if(images[startImageNo].cursorPos!=0)
            retrieveOnRowDown();
            rowDownNeed=false;
        }

        for(int i =0; i< totalImagesToDraw; i++)
        {
            GLES31.glUniform1i(5,i);
            int imageIndex=i+startImageNo;
            if(imageIndex<totalImagesToDraw)
            images[imageIndex].draw();
            else
                images[imageIndex-totalImagesToDraw].draw();

            Graphics.checkGLError("onDraw");
            //testImage.draw();
        }

        // Log.e("ImageNO",mediaCursor.getPosition() + "of total : " + mediaCursor.getCount());
        Graphics.checkGLError("onDraw");


    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {//////GL10?
        programId=createGlProgram("randomeForNow","randome for now");//this is actually source;
        GLES30.glUseProgram(programId);
        Image.mProgram=programId;
        getMediaCursor();////on worker thread
        Log.e("cursor imagecount",mediaCursor.getCount() + "");
        //mediaCursor.moveToPosition(-2);
        Image.context=context;
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {///////GL10?
        this.width=width;
        this.height=height;
        GLES31.glViewport(0, 50, width, height-startY);/////needs to set right dims
        numRows=(height)/(width/numColoumns);//square image same height and width make is screenDensity Indendent;// division may give one row less sometimes
        maxImageWidth=(width-(numColoumns-1)*gap)/numColoumns;//d
        maxImageHeight=maxImageWidth;
        totalImagesToDraw =numColoumns*(numRows+2);
        Log.e("TotalIamgedrawCOunt ","" + totalImagesToDraw);
        images=new Image[totalImagesToDraw];
        for(int i = 0; i< totalImagesToDraw; i++)
        {
            images[i]=new Image();//////blank cauz images are not loaded to textures;
            images[i].setBounds(0,0,width/numColoumns,width/numColoumns);

        }
        //calcuateImageTextureAndBound();called when drawing;


    }
    public void  getMediaCursor()
    {
        Uri collection;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {//////check this is correct
            collection = MediaStore.Images.Media.getContentUri(MediaStore.VOLUME_EXTERNAL);
        } else {
            collection = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
        }
        try
        {/////do the query in workerTHread
            Log.e("getCursor","TEst");

            mediaCursor=context.getContentResolver().query(collection,projection,selection,selectionArgs,sortOrder);
            Log.e("getCursor","TEst");

        }
        catch (Exception e){e.printStackTrace();}
    }
    public void loadImageFromCursor(Image image)
    {
        if(image.bitmap!=null)
        image.bitmap.recycle();
        if(mediaCursor.getPosition()<mediaCursor.getCount()-1)
        {
            mediaCursor.moveToNext();
        }
        if(mediaCursor!=null) {//try?
            int idColumn = mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media._ID);
            int nameColumn = mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media.TITLE);
            int relativPathId=mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);//use relative path or displayname
            // int sizeColumn=mediaCursor.getColumnIndex(MediaStore.Images.Media.SIZE);
            //while(mediaCursor.moveToNext()){}
            long id = mediaCursor.getLong(idColumn);
            String name = mediaCursor.getString(nameColumn);
            String relativePath=mediaCursor.getString(relativPathId);
          //  Log.e("Cursor image","belog to relativepath"+relativePath);

            // int size=mediaCursor.getInt(sizeColumn);
            Uri contentUri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id);
            try {
////Load screenDensity independent
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                    image.bitmap = context.getContentResolver().loadThumbnail(contentUri, new Size((int)maxImageWidth, (int)maxImageHeight), null);//change method to obtain thumbnail
                    //testImage.bitmap=crop(testImage[i].bitmap);
                    //acturally image should fit to center of maxImageHeight and width

                } else {

                    image.bitmap = MediaStore.Images.Thumbnails.getThumbnail(context.getContentResolver(), id, MediaStore.Images.Thumbnails.MINI_KIND, null);
                    if (image.bitmap != null) { //testImage.bitmap=crop(testImage[i].bitmap);
                    }
                }
                if (image.bitmap == null) {
                    // InputStream stream = context.getContentResolver().openInputStream(testImage[i].imageUri);
                    //testImage[i].bitmap = BitmapFactory.decodeStream(stream);
                    //testImage[i].bitmap = Bitmap.createScaledBitmap(testImage.bitmap,thumbnailWidth,thumbnailHeight,false);
                    Log.e("Thumnail loading", "erro null bitmap");
                   // image.bitmap= BitmapFactory.decodeResource(context.getResources(),R.drawable.cars);

                }

            } catch (Exception e) {
                e.printStackTrace();
            }

            image.resetTexImage();
            image.cursorPos=mediaCursor.getPosition();
           // Log.e("setImages","Texture reset in setIamgesFromCursor");


        }

     //   Log.e("curosor position",mediaCursor.getPosition() + "set images from cursor");
    }
    public void calcuateImageTextureAndBound()
    {
        for(int i = 0; i< totalImagesToDraw; i++)
        {
            loadImageFromCursor(images[i]);
            cursorPosLastImage=mediaCursor.getPosition();
        }
    }
    public void onTouch(View v, MotionEvent event)//Need ReturnType;
    {
        int pointerIndex=(event.getAction()&MotionEvent.ACTION_POINTER_INDEX_MASK)>>MotionEvent.ACTION_POINTER_INDEX_SHIFT;
        ///return needed;
        int tempPointerId=event.getPointerId(pointerIndex);
        float touchX=event.getX(),touchY=event.getY();
        switch (event.getAction())
        {
            case MotionEvent.ACTION_DOWN:
            {
                Log.e("ontouchRenderer","initial touch touched");
                pointerId=event.getPointerId(pointerIndex);
                initialTouchX=touchX;
                initialTouchY=touchY;
                previousTouchX=touchX;
                previousTouchY=touchY;
                if(touchVelocityTracker==null)
                {
                    touchVelocityTracker=VelocityTracker.obtain();
                }
                else
                {
                    touchVelocityTracker.clear();
                }
                touchVelocityTracker.addMovement(event);


            }break;
            case MotionEvent.ACTION_POINTER_DOWN:
            {

            }break;
            case MotionEvent.ACTION_MOVE:
            {
                int pointerCount=event.getPointerCount();
                for(int i=0;i<pointerCount;i++)
                {
                    touchVelocityTracker.addMovement(event);
                    tempPointerId=event.getPointerId(i);
                    if(tempPointerId==pointerId)
                    {
                      //  Log.e("ontouchRenderer","Renderer TOuch Moved");
                        touchX=event.getX();
                        touchY=event.getY();
                     //   float moveDisX=previousTouchX-touchX;//only moved along y
                        float moveDisY=previousTouchY-touchY;
                        totalMoveDisY+=moveDisY;
                        onMove(moveDisY);
                       // imageUpdateNeeded=true;

                        previousTouchX=touchX;
                        previousTouchY=touchY;

                        touchVelocityTracker.computeCurrentVelocity(100);//log every second(1000ms);expensive so call every second
                       // Log.e("Velocity", "X velocity: " + touchVelocityTracker.getXVelocity(pointerId));
                        yTouchVelocity=touchVelocityTracker.getYVelocity(pointerId);
                        deceleration=yTouchVelocity/decelerationTimeInFrames;

                        Log.e("Velocity", "Y velocity: " + yTouchVelocity);
                    }


                }
            }
            break;
            case MotionEvent.ACTION_POINTER_UP:////move pointer up action up to function;
            {
                if(tempPointerId==pointerId)
                {
                    pointerId=-1;
                    Log.e("ontouchRenderer","pointerUP");
                    if(yTouchVelocity>=0)
                        velocitySignPositive=true;
                    else velocitySignPositive=false;

                    if(deceleration!=0)//should check aginst already scrolling not decleation;
                    {
                        decelerationTimeInFrames+=20;
                        deceleration=yTouchVelocity/decelerationTimeInFrames;///decelate to 0 speed in 200 frames(use time instead of frames);
                        ///if continuous scrolling increase frameCount or time to decelearate so as to scroll more;
                    }

                }//setting pointer to invalid

            }break;
            case MotionEvent.ACTION_UP:
            {
                if(tempPointerId==pointerId)
                {
                    pointerId=-1;
                    Log.e("ontouchRenderer","ActionUP");
                    if(yTouchVelocity>=0)
                        velocitySignPositive=true;
                    else velocitySignPositive=false;

                    if(deceleration!=0)//already scrolling
                    {
                        decelerationTimeInFrames+=20;
                        deceleration=yTouchVelocity/decelerationTimeInFrames;///decelate to 0 speed in 200 frames(use time instead of frames);
                        ///if continuous scrolling increase frameCount or time to decelearate so as to scroll more;
                    }
                }//setting pointer to invalid
            }
            break;
            case MotionEvent.ACTION_CANCEL:
            {
                touchVelocityTracker.recycle();
            }
            break;
        }

       // imageUpdateNeeded=true;

    }
    public void onMove(float moveDisY)
    {

        //instead of below condition make it set to 0 positoin on touchup
        Log.e("move distance","during movie is " + moveDisY);
        if(!(moveDisY<0&&images[startImageNo].cursorPos==0&&images[startImageNo].startY>=-0.1*images[startImageNo].height))
            for(int k=0;k<totalImagesToDraw;k++)
            {
                images[k].setBounds(images[k].startX,images[k].startY-moveDisY,images[k].width,images[k].height);
                ///currently all images have same bounds only need for one acturally(draw using one draw call);
            }
        // if(totalMoveDisY>(images[0].height))
        if(images[0].startY<-(images[0].height*1.0))//1.5 image down//make 1.0 to 1.5 if error in dims;
        {

            rowUp();
            //imageUpdateNeeded=true;
            totalMoveDisY=0.0f;/////is totalMoveDisY needed any move
           // moveDisForVelocity=0.0f;//if called from drawFrame based on velocity
        }
        else if(images[0].startY>0)
        {
            rowDown();
            //imageUpdateNeeded=true;
            totalMoveDisY=0.0f;
           // moveDisForVelocity=0.0f;
        }
    }
    public void rowUp()
    {//check error if images are completed
        //imageUpdateNeeded=true;
        rowupNeed=true;/////cannot modify images directly here as it is called from ChooserView not the opengl thread.

        //the topmost row will be now drawn on bottom but new images are set in them

        Log.e("ROW","UP" +totalMoveDisY);
        for(int i=0;i<totalImagesToDraw;i++)
        {
            float tempStartY=images[i].startY+maxImageHeight;
            images[i].setBounds(images[i].startX,tempStartY,images[i].width,images[i].height);
        }

    }
    public void rowDown()
    {
        Log.e("Row","down");
        rowDownNeed=true;
        for(int i=0;i<totalImagesToDraw;i++)
        {
            float tempStartY=images[i].startY-maxImageHeight;
            images[i].setBounds(images[i].startX,tempStartY,images[i].width,images[i].height);
        }

    }
    public void retrieveOnRowUp()
    {
        int lastRowStartIndex=startImageNo-numColoumns;
        if(lastRowStartIndex<0)
        {
            lastRowStartIndex=totalImagesToDraw+lastRowStartIndex;
        }
        Log.e("the lastROw start index", "" + lastRowStartIndex);
        mediaCursor.moveToPosition((int)images[lastRowStartIndex].cursorPos+numColoumns-1);
        for(int i=0;i<numColoumns;i++)
        {
           loadImageFromCursor(images[i+startImageNo]);/////do this on other thread
        }
        startImageNo+=numColoumns;
        if(startImageNo>totalImagesToDraw-1)
        {
            startImageNo=startImageNo-totalImagesToDraw;
        }
    }
    public void retrieveOnRowDown()
    {
        mediaCursor.moveToPosition((int)images[startImageNo].cursorPos-numColoumns-1);
        int lastRowStartIndex=startImageNo-numColoumns;
        if(lastRowStartIndex<0)
        {
            lastRowStartIndex=totalImagesToDraw+lastRowStartIndex;
        }
        Log.e("the lastROw start index", "" + lastRowStartIndex);
        for(int i=0;i<numColoumns;i++)
        {

            loadImageFromCursor(images[lastRowStartIndex+i]);/////do this on other thread
        }
        startImageNo=lastRowStartIndex;
    }
    void setStartLocation(int x,int y)
    {
        startX=x;
        startY=y;
    }

}
