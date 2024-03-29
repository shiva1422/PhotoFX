package com.kalasoft.photofx;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.BitmapFactory;
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
/*/
   for now all images have same dimension except for startY
   so nay calculation based on image Dimesntion should be carefully done
   //includes selectImageOnClick,draw(),
   optimize later;
   Shoud have done images as array instead of continuous single
   Optimize onMoveMethod for good scrolling and performance;
 */
    static
    {
        System.loadLibrary("main");
    }
    static float width, height,startX=0,startY=0;
    public int programId=0;
    Context context;

    //Media Cursor
    private boolean imageUpdateNeeded=true,rowupNeed=false,rowDownNeed=false,cursorAtTheEnd=false;
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
    private int totalImagesToDraw,startImageNo=0;//to display
    private int totalActiveView=0,numRows=0,numColoumns=3;
    private float maxImageHeight,maxImageWidth;
    private float gap=5.0f;//gap in pixels
    Image[] images;


    //shaderLayout Locations
    private int imageNumLoc=0,displayParamsLoc=0,matrixSizeLoc=0,gapLoc=0;

    //Touch
    private int pointerId,decelerationTimeInFrames=100;
    private boolean velocitySignPositive=false,shouldDecelerate=false,isScrolling=false,bOverMoveOnTop=false;
    private float yTouchVelocity=0.0f,deceleration=0;
    private float initialTouchX=0.0f,initialTouchY=0.0f,previousTouchX=0.0f,previousTouchY=0.0f,moveDisForVelocity=0.0f;
    private VelocityTracker touchVelocityTracker=null;

    //Imge selection;
    boolean bSelectionDone=false;
    Image selectedImage=null;


ChooserRenderer(Context context)
{
    super();
    this.context = context;
}
public native int createGlProgram(String vertexShaderSource,String fragmentShaderSource);

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {//////GL10?
        programId=createGlProgram("randomeForNow","randome for now");
        GLES31.glUseProgram(programId);                                                             //do this in onsurface changed;
        Image.mProgram=programId;
        getMediaCursor();                                                                           /////on worker thread
        Image.context=context;

        displayParamsLoc=GLES31.glGetUniformLocation(programId,"displayParams");             //use hardcoded instead?
        imageNumLoc=GLES31.glGetUniformLocation(programId,"imageNum");
        matrixSizeLoc=GLES31.glGetUniformLocation(programId,"numCols");
        gapLoc=GLES31.glGetUniformLocation(programId,"gap");
    }


    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {///////GL10?

        this.width=width;
        this.height=height;
        GLES31.glUseProgram(programId);
        GLES31.glViewport(0, 0, width, (int)(height-startY));                               //needs to set right dims
        numRows=(height)/(width/numColoumns);                                                       //square image same height and width make is screenDensity Indendent;// division may give one row less sometimes
        maxImageWidth=(this.width-(numColoumns-1)*gap)/numColoumns;
        maxImageHeight=(this.height-(numRows-1)*gap)/numRows;
        totalImagesToDraw =numColoumns*(numRows+2);
        images=new Image[totalImagesToDraw];
        for(int rowNo=0;rowNo<(numRows+2);rowNo++)
        {
            for(int colNo=0;colNo<numColoumns;colNo++)
            {
                images[rowNo*(numColoumns)+colNo]=new Image();
                images[rowNo*(numColoumns)+colNo].setBounds(startX,startY,maxImageWidth,maxImageHeight);///this should be defeern for different images

            }
        }
        onMove(0.001f);//just to position currenctly on setup;

        for(int i = 0; i< totalImagesToDraw; i++)
        {
            loadImageFromCursor(images[i]);
            cursorPosLastImage=mediaCursor.getPosition();
        }

        selectedImage=new Image();
    }


    @Override
    public void onDrawFrame(GL10 gl)
    {
        GLES31.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES31.glClear(GLES31.GL_COLOR_BUFFER_BIT);
        GLES31.glUniform1f(gapLoc,gap);//three lines need not be in every frame
        GLES31.glUniform1i(matrixSizeLoc,numColoumns);
        GLES31.glUniform2f(displayParamsLoc,width,height);
        if(!bSelectionDone)
        {
            if(yTouchVelocity!=0)
            {
                autoScroll();
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
            }
        }
        else
        {
            GLES31.glUniform1i(5,0);
            selectedImage.draw();
        }

        Graphics.checkGLError("onDraw");
    }


    public void  getMediaCursor()
    {
        Uri collection;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
        {//////check this is correct
            collection = MediaStore.Images.Media.getContentUri(MediaStore.VOLUME_EXTERNAL);
        }
        else
            {
            collection = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
         }
        try
        {/////do the query in workerTHread
            mediaCursor=context.getContentResolver().query(collection,projection,selection,selectionArgs,sortOrder);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            Log.e("ChooserView","getMediaCursor COuld not get");
        }
    }
    public void loadImageFromCursor(Image image)
    {
        if(mediaCursor==null)
        {
            return;
        }
        if(image.bitmap!=null)
        image.bitmap.recycle();
        if(mediaCursor.getPosition()>=(mediaCursor.getCount()-1))
        {//load default image non loadable image;; this generally is for last row with images les than numColoimns
            try
            {
                image.bitmap= BitmapFactory.decodeResource(context.getResources(),R.drawable.cursordefaultimage);
                image.bLoadableImage=false;
                image.resetTexImage();
                image.cursorPos=mediaCursor.getPosition();///this is just for convinience and not actual postion for this image;
            }
            catch (Exception e)
            {
                e.printStackTrace();
            }
            return;
        }

            mediaCursor.moveToNext();//try?
            int idColumn = mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media._ID);
            int nameColumn = mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media.TITLE);
            int relativPathId=mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);//use relative path or displayname
            //int sizeColumn=mediaCursor.getColumnIndex(MediaStore.Images.Media.SIZE);
            long id = mediaCursor.getLong(idColumn);
            String name = mediaCursor.getString(nameColumn);
            String relativePath=mediaCursor.getString(relativPathId);
            //Log.e("Cursor image","belog to relativepath"+relativePath);
            // int size=mediaCursor.getInt(sizeColumn);
            Uri contentUri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id);
            try
            {
                ////Load screenDensity independent
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
                {
                    image.bitmap = context.getContentResolver().loadThumbnail(contentUri, new Size((int)maxImageWidth, (int)maxImageHeight), null);//change method to obtain thumbnail
                    //testImage.bitmap=crop(testImage[i].bitmap);
                    //acturally image should fit to center of maxImageHeight and width

                } else
                    {

                    image.bitmap = MediaStore.Images.Thumbnails.getThumbnail(context.getContentResolver(), id, MediaStore.Images.Thumbnails.MINI_KIND, null);
                    if (image.bitmap != null) { //testImage.bitmap=crop(testImage[i].bitmap);
                    }
                }
                if (image.bitmap == null)
                {
                    // InputStream stream = context.getContentResolver().openInputStream(testImage[i].imageUri);
                    //testImage[i].bitmap = BitmapFactory.decodeStream(stream);
                    //testImage[i].bitmap = Bitmap.createScaledBitmap(testImage.bitmap,thumbnailWidth,thumbnailHeight,false);
                    Log.e("Thumnail loading", "erro null bitmap");
                   // image.bitmap= BitmapFactory.decodeResource(context.getResources(),R.drawable.cars);

                }

            }
            catch (Exception e)
            {
                e.printStackTrace();
            }

            image.resetTexImage();
            image.cursorPos=mediaCursor.getPosition();
            image.bLoadableImage=true;
           // Log.e("setImages","Texture reset in setIamgesFromCursor");

    }

    public void onTouch(View v, MotionEvent event)//Need ReturnType;////this is not done on GLThread so can crash ad draw and onMove are called on different threads at same time
    {
        int pointerIndex=(event.getAction()&MotionEvent.ACTION_POINTER_INDEX_MASK)>>MotionEvent.ACTION_POINTER_INDEX_SHIFT;
        int tempPointerId=event.getPointerId(pointerIndex);
        float touchX=event.getX(),touchY=event.getY();
        switch (event.getAction())
        {
            case MotionEvent.ACTION_DOWN:
            {
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
                touchVelocityTracker.addMovement(event);
                for(int i=0;i<pointerCount;i++)
                {
                    tempPointerId=event.getPointerId(i);
                    if(tempPointerId==pointerId)
                    {
                        touchX=event.getX();
                        touchY=event.getY();
                        float moveDisY=previousTouchY-touchY;
                        onMove(moveDisY);//////crashes as this touch method is differnt from rendering thread;
                        previousTouchX=touchX;
                        previousTouchY=touchY;
                        touchVelocityTracker.computeCurrentVelocity(100);//log every second(1000ms);expensive so call every second
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
                    float tempMoveDisx=touchX-initialTouchX;
                    float tempMoveDisy=touchY-initialTouchY;
                    if(tempMoveDisx<=5&&tempMoveDisx>=-5&&tempMoveDisy<=5&&tempMoveDisy>=-5)//if not movemuch selcted
                    {//cannot be done here as differnt thread
                        selectImageOnClick(touchX,touchY);
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
                    float tempMoveDisx=touchX-initialTouchX;
                    float tempMoveDisy=touchY-initialTouchY;
                    if(tempMoveDisx<=5&&tempMoveDisx>=-5&&tempMoveDisy<=5&&tempMoveDisy>=-5)//if not movemuch selcted
                    {//cannot be done here as differnt thread
                        selectImageOnClick(touchX,touchY);
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
        //instead of below condition make it set to 0 positoin on touchup when the cursor is at top or 1st image and movedown;
        int remainingImagesInCursor=mediaCursor.getCount()-mediaCursor.getPosition()-1;
        if(!(moveDisY<0&&images[startImageNo].cursorPos==0&&images[startImageNo].startY>=-0.1*images[startImageNo].height)&&(remainingImagesInCursor>0||moveDisY<0))
            for(int k=0;k<totalImagesToDraw;k++)
            {
                images[k].setBounds(images[k].startX,images[k].startY-moveDisY,images[k].width,images[k].height);
            }
        else if(remainingImagesInCursor==0&&images[0].startY>-1.0*maxImageHeight)
        {
            //cannot load additional imagages but can be moved(extra rows);when cursor is at end
            for(int k=0;k<totalImagesToDraw;k++)
            {
                images[k].setBounds(images[k].startX,images[k].startY-moveDisY,images[k].width,images[k].height);
            }
            return;
        }
        if(images[0].startY<-(maxImageHeight*1.0)&&remainingImagesInCursor>0)//1.5 image down//make 1.0 to 1.5 if error in dims;
        {
            //row up should not happen if the cursor is the end of;

            rowupNeed=true;                                                  /////cannot modify images directly here as it is called from ChooserView not the opengl thread.
            for(int i=0;i<totalImagesToDraw;i++)                            //the topmost row will be now drawn on bottom but new images are set in them
                {
                    float tempStartY=images[i].startY+maxImageHeight;
                    images[i].setBounds(images[i].startX,tempStartY,images[i].width,images[i].height);
                }
            // moveDisForVelocity=0.0f;//if called from drawFrame based on velocity
        }
        else if(images[0].startY>0)
        {
            Log.e("Row","down");
            rowDownNeed=true;
            for(int i=0;i<totalImagesToDraw;i++)
            {
                float tempStartY=images[i].startY-maxImageHeight;
                images[i].setBounds(images[i].startX,tempStartY,images[i].width,images[i].height);
            }
            //imageUpdateNeeded=true;
           // moveDisForVelocity=0.0f;
        }
    }
    public void retrieveOnRowUp()
    {
       int remainingImagesInCursor=mediaCursor.getCount()-mediaCursor.getPosition()-1;
      if(remainingImagesInCursor>0)
      {
          int lastRowStartIndex = startImageNo - numColoumns;
          if (lastRowStartIndex < 0) {
              lastRowStartIndex = totalImagesToDraw + lastRowStartIndex;
          }
          Log.e("the lastROw start index", "" + lastRowStartIndex);
          mediaCursor.moveToPosition((int) images[lastRowStartIndex].cursorPos + numColoumns - 1);             //no error in cursor as moved to existing pos;
          for (int i = 0; i < numColoumns; i++)
          {
              loadImageFromCursor(images[i + startImageNo]);/////do this on other thread
          }
          startImageNo += numColoumns;
          if (startImageNo > totalImagesToDraw - 1) {
              startImageNo = startImageNo - totalImagesToDraw;
          }
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
    private void autoScroll()
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
    void setStartLocation(float x,float y)
    {
        startX=x;
        startY=y;
    }
    void selectImageOnClick(float x,float y)
    {
        for(int rowNo=0;rowNo<(numRows+2);rowNo++)//+2 cuz two extra rows
        {
            for (int colNo = 0; colNo < numColoumns; colNo++)
            {
                float picstartX=colNo*(maxImageWidth+gap);
                float picstartY=images[startImageNo].startY+(rowNo+1)*maxImageHeight;
                if(x>=picstartX&&x<=(picstartX+maxImageWidth)&&y>=picstartY&&y<=(picstartY+maxImageHeight))
                {
                    int imageIndex=rowNo*(numColoumns)+colNo+startImageNo;
                    if(imageIndex<totalImagesToDraw)
                    {
                        selectedImage=images[imageIndex];
                    }
                    else
                    {
                        selectedImage=images[imageIndex-totalImagesToDraw];

                    }
                    selectedImage.setBounds(100,100,maxImageWidth,maxImageHeight);
                    bSelectionDone=true;
                    Log.e("Image Selcected " ,"no is "+ imageIndex );

                    break;

                }
                if(bSelectionDone)
                    break;
            }
        }
    }

}
