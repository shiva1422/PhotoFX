package com.kalasoft.photofx;

import android.content.ContentUris;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.opengl.GLES20;
import android.opengl.GLES30;
import android.opengl.GLES31;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.provider.MediaStore;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Size;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Arrays;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ChooserRenderer implements GLSurfaceView.Renderer {
    static int width, height;
    public int programId=0;
    Image testImage;
    static float red=0.0f;
    static int test=0;
    static {
        System.loadLibrary("main");
    }
    Context context;

    ///MEdiaCursor;
    //Media Cursor
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
    private String sortOrder =MediaStore.Images.Media.DATE_ADDED + " ASC";


ChooserRenderer(Context context) {
    this.context = context;
    //no need as we get width and height on surface created;
   // WindowManager windowManager=(WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
    //windowManager.getDefaultDisplay().getRealMetrics(displayParams);//api dependent //windowManager.getCurrentWindowMetric


}


    @Override
    public void onDrawFrame(GL10 gl) {
      // Log.e("RendererChooser:draw","the prog id" + programId);
        GLES31.glUseProgram(programId);
        red+=0.05f;if(red>1){red=0;}
       // Log.e("PROGRAMID","" +programId);
        GLES31.glClearColor(0.4f, 0.0f, 0.0f, 1.0f);
        GLES31.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        checkGLError("ChooserRenderre");
     //  triangle.draw();
        testImage.draw();
         Log.e("ImageNO",mediaCursor.getPosition() + "of total : " + mediaCursor.getCount());
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
            Log.e("Cursor image","belog to relativepath"+relativePath);

            // int size=mediaCursor.getInt(sizeColumn);
            Uri contentUri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id);
            try {
////Load screenDensity independent
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
                    testImage.bitmap = context.getContentResolver().loadThumbnail(contentUri, new Size(256, 256), null);//change method to obtain thumbnail
                    //testImage.bitmap=crop(testImage[i].bitmap);

                } else {

                    testImage.bitmap = MediaStore.Images.Thumbnails.getThumbnail(context.getContentResolver(), id, MediaStore.Images.Thumbnails.MINI_KIND, null);
                    if (testImage.bitmap != null) { //testImage.bitmap=crop(testImage[i].bitmap);
                    }
                }
                if (testImage.bitmap == null) {
                    // InputStream stream = context.getContentResolver().openInputStream(testImage[i].imageUri);
                    //testImage[i].bitmap = BitmapFactory.decodeStream(stream);
                    //testImage[i].bitmap = Bitmap.createScaledBitmap(testImage.bitmap,thumbnailWidth,thumbnailHeight,false);
                    Log.e("Thumnail loading", "erro null bitmap");

                }

            } catch (Exception e) {
                e.printStackTrace();
            }

            testImage.resetTexImage();


        }

    }
    public void checkGLError(String tag) {
        int error;
        if ((error = GLES31.glGetError()) != GLES31.GL_NO_ERROR) {
            Log.e("RenderErrorglCheck", tag + ": glError " + error);
        }
    }
    public native int createGlProgram(String vertexShaderPath,String fragmentShaderPath);
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {//////GL10?
        programId=createGlProgram("sdf","dfs");//check if pogramcreated;
        GLES30.glUseProgram(programId);
        testImage=new Image();
        //testImage.setBounds(0,0,width/3,width/3);
        Image.mProgram=programId;

        getMediaCursor();////on worker thread
       // Log.e("ImageCount",""+mediaCursor.getCount());
        mediaCursor.moveToNext();//shuld be before accessigng

        //  mediaCursor.moveToPosition(8);
        if(mediaCursor!=null)
        {//try?
            int idColumn = mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media._ID);
            int nameColumn =mediaCursor.getColumnIndexOrThrow(MediaStore.Images.Media.TITLE);
           // int sizeColumn=mediaCursor.getColumnIndex(MediaStore.Images.Media.SIZE);
            //while(mediaCursor.moveToNext()){}
            long id=mediaCursor.getLong(idColumn);
            String name=mediaCursor.getString(nameColumn);

           // int size=mediaCursor.getInt(sizeColumn);
            Uri contentUri = ContentUris.withAppendedId(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, id);
            try {
                //below line api level dependent
             //   square.image = context.getContentResolver().loadThumbnail(contentUri, new Size(100, 100), null);
               testImage.bitmap= MediaStore.Images.Media.getBitmap(context.getContentResolver(), contentUri);

            }catch (Exception e){e.printStackTrace();}

            if (testImage == null) {
                Log.e("DFDFDFD", "DFDFD");

            }
        }
        testImage.context=context;
        testImage.setTexImage();

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {///////GL10?
        this.width=width;
        this.height=height;
        testImage.setBounds(0,0,width/3,width/3);
        GLES31.glViewport(0, 0, width, height);
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


}
