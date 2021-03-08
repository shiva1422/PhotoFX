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
import android.view.View;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ChooserRenderer implements GLSurfaceView.Renderer {/////////Opengl updates should be done on the thread only for example onTouch caled from ChooserView so can do image uploads therer
    static int width, height;
    public int programId=0;
   // Image testImage;
    Image[] images;

    private int totalImagesToDraw,startImageNo=0;//to display
    static float red=0.0f;
    static int test=0;
    static {
        System.loadLibrary("main");
    }
    Context context;

    ///MEdiaCursor;
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
    private int pointerId;
   private float initialTouchX=0.0f,initialTouchY=0.0f,previousTouchX=0.0f,previousTouchY=0.0f,totalMoveDisY=0.0f;


ChooserRenderer(Context context) {
    this.context = context;
    //no need as we get width and height on surface created;
   // WindowManager windowManager=(WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
    //windowManager.getDefaultDisplay().getRealMetrics(displayParams);//api dependent //windowManager.getCurrentWindowMetric


}
int testNo=0;

    @Override
    public void onDrawFrame(GL10 gl) {
      // Log.e("RendererChooser:draw","the prog id" + programId);
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

    public native int createGlProgram(String vertexShaderPath,String fragmentShaderPath);
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {//////GL10?
        programId=createGlProgram("sdf","dfs");//check if pogramcreated;
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
        GLES31.glViewport(0, 0, width, height);
        numRows=(height)/(width/numColoumns);//square image same height and width make is screenDensity Indendent;// division may give one row less sometimes
        maxImageWidth=(width-(numColoumns-1)*gap)/numColoumns;//d
        maxImageHeight=maxImageWidth;
        totalImagesToDraw =numColoumns*(numRows+2);
        Log.e("TotalIamgedrawCOunt ","" + totalImagesToDraw);
        images=new Image[totalImagesToDraw];
        for(int i = 0; i< totalImagesToDraw; i++)
        {
            images[i]=new Image();
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

            }break;
            case MotionEvent.ACTION_POINTER_DOWN:
            {

            }break;
            case MotionEvent.ACTION_MOVE:
            {
                int pointerCount=event.getPointerCount();
                for(int i=0;i<pointerCount;i++)
                {
                    tempPointerId=event.getPointerId(i);
                    if(tempPointerId==pointerId)
                    {
                      //  Log.e("ontouchRenderer","Renderer TOuch Moved");
                        touchX=event.getX();
                        touchY=event.getY();
                     //   float moveDisX=previousTouchX-touchX;//only moved along y
                        float moveDisY=previousTouchY-touchY;
                        totalMoveDisY+=moveDisY;
                        //instead of below condition make it set to 0 positoin on touchup
                        if(!(moveDisY<0&&images[startImageNo].cursorPos==0&&images[startImageNo].startY>=-0.1*images[startImageNo].height))
                        for(int k=0;k<totalImagesToDraw;k++)
                        {
                            images[k].setBounds(images[k].startX,images[k].startY-moveDisY,images[k].width,images[k].height);
                            ///currently all images have same bounds only need for one acturally(draw using one draw call);
                        }
                        // if(totalMoveDisY>(images[0].height))
                        if(images[0].startY<-(images[0].height*1.5))//11/2 image down
                        {

                            rowUp();
                            //imageUpdateNeeded=true;
                            totalMoveDisY=0.0f;
                        }
                        else if(images[0].startY>0)
                        {
                            rowDown();
                            //imageUpdateNeeded=true;
                            totalMoveDisY=0.0f;
                        }
                       // imageUpdateNeeded=true;

                        previousTouchX=touchX;
                        previousTouchY=touchY;
                    }



                }


            }
            break;
            case MotionEvent.ACTION_POINTER_UP:
            {
                if(tempPointerId==pointerId)
                {pointerId=-1;
                    Log.e("ontouchRenderer","pointerUP");
                }//setting pointer to invalid
            }break;
            case MotionEvent.ACTION_UP:
            {
                if(tempPointerId==pointerId)
                {
                    pointerId=-1;
                    Log.e("ontouchRenderer","ActionUP");
                }//setting pointer to invalid
            }
            break;
        }

       // imageUpdateNeeded=true;

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


}
