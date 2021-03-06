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
import android.view.View;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ChooserRenderer implements GLSurfaceView.Renderer {
    static int width, height;
    public int programId=0;
   // Image testImage;
    Image[] images;
    private int totalImagesToDraw;//to display
    static float red=0.0f;
    static int test=0;
    static {
        System.loadLibrary("main");
    }
    Context context;

    ///MEdiaCursor;
    //Media Cursor
    private boolean imageUpdateNeeded=true;
    private Cursor mediaCursor;
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
    private int maxImageHeight,maxImageWidth;
    private int totalActiveView=0,numRows=0,numColoumns=7;

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
        GLES31.glClearColor(0.4f, 0.0f, 0.0f, 1.0f);
        GLES31.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        Graphics.checkGLError("ChooserRenderre");
        int imageNumLoc=GLES31.glGetUniformLocation(programId,"imageNum");
        int matrixSizeLoc=GLES31.glGetUniformLocation(programId,"numCols");
        GLES31.glUniform1i(matrixSizeLoc,numColoumns);
        if(imageUpdateNeeded)
        {
            calcuateImageTextureAndBound();
            imageUpdateNeeded=false;
        }
        for(int i = 0; i< totalImagesToDraw; i++)
        {
            //if(imageUpdateNeeded)
            images[i].draw();
            Graphics.checkGLError("onDraw");
            GLES31.glUniform1i(5,i);
            //testImage.draw();
        }
        //testImage.draw();
        // Log.e("ImageNO",mediaCursor.getPosition() + "of total : " + mediaCursor.getCount());

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
        numRows=(height)/(width/numColoumns);//square image same height and width make is screenDensity Indendent;
        maxImageWidth=width/numColoumns;//d
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
    public void setImagesToDraw(Image image)
    {
        if(image.bitmap!=null)
        image.bitmap.recycle();
        if(mediaCursor.getPosition()<mediaCursor.getCount()-1)
            mediaCursor.moveToNext();
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
                    image.bitmap = context.getContentResolver().loadThumbnail(contentUri, new Size(maxImageWidth, maxImageHeight), null);//change method to obtain thumbnail
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


        }
        Log.e("curosor position",mediaCursor.getPosition() + "");

    }
    public void calcuateImageTextureAndBound()
    {
        for(int i = 0; i< totalImagesToDraw; i++)
        {
            setImagesToDraw(images[i]);
        }
    }
    public void onTouch(View v, MotionEvent event)
    {
        ///return needed;
        Log.e("ontouchRenderer","touched");
        if(event.getAction()==MotionEvent.ACTION_DOWN)
        imageUpdateNeeded=true;
    }


}
