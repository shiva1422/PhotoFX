package com.kalasoft.photofx;

import androidx.appcompat.app.AppCompatActivity;

import android.app.NativeActivity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;

public class EditorActivity extends NativeActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
    float[] getDisplayParams()
    {
        float[] displayParams=new float[8];
        DisplayMetrics displayMetrics=new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getRealMetrics(displayMetrics);
        displayParams[0]=displayMetrics.widthPixels;
        displayParams[1]=displayMetrics.heightPixels;
        displayParams[2]=displayMetrics.density;
        displayParams[3]=displayMetrics.densityDpi;
        displayParams[4]=displayMetrics.DENSITY_DEVICE_STABLE;
        displayParams[5]=displayMetrics.scaledDensity;
        displayParams[6]=displayMetrics.xdpi;
        displayParams[7]=displayMetrics.ydpi;
        return displayParams;

    }
    Bitmap getBitmap(int id)
    {
        int bitmapId=0;
        Log.e("getBitmap","the id is " + id);
        switch (id)
        {
            case 0:
               bitmapId=R.drawable.cars;
                break;
            case 1:
               // bitmapId=R.drawable.ssnare;
                break;
            case 2:
                //bitmapId=R.drawable.stom;
                break;
            case 3:
                bitmapId=R.drawable.bottle;
                break;
            default:
               bitmapId= R.drawable.kala;
        }
        Bitmap bitmap=null;
        try {
            // InputStream stream = getContentResolver().openInputStream(imageUri);
            BitmapFactory.Options options=new BitmapFactory.Options();
            options.inScaled=false;//scaling doesnt depend on screenDensity
            //  BitmapFactory.decodeResource(getResources(),bitmapId,options);
            // Log.e("bitmapDims","height:"+options.outHeight+"width:"+options.outWidth);
            bitmap= BitmapFactory.decodeResource(getResources(),bitmapId,options);
        }catch (Exception e)
        {e.printStackTrace();}


        return bitmap;
    }

    public void hideSystemUI()
    {

        // Enables regular immersive mode.
        // For "lean back" mode, remove SYSTEM_UI_FLAG_IMMERSIVE.
        // Or for "sticky immersive," replace it with SYSTEM_UI_FLAG_IMMERSIVE_STICKY

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {

                    View decorView = getWindow().getDecorView();
                    decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                            // Set the content to appear under the system bars so that the
                            // content doesn't resize when the system bars hide and show.
                            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            // Hide the nav bar and status bar
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN);
                }catch (Exception e){e.printStackTrace();}

            }
        });


    }
    public void showSystemUI()
    {
        View decorView=getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE|View.SYSTEM_UI_FLAG_HIDE_NAVIGATION|View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
    }
    private void changeVolume(boolean increase)
    {
        AudioManager audioManager=(AudioManager)getApplicationContext().getSystemService(getApplicationContext().AUDIO_SERVICE);

        if(increase)
        {

            Log.e("Java ","increasevbolume");
            audioManager.adjustVolume(AudioManager.ADJUST_RAISE,AudioManager.FLAG_SHOW_UI);
        }
        else
        {
            //decrease volume
            Log.e("Java ","decreasevbolume");
            audioManager.adjustVolume(AudioManager.ADJUST_LOWER,AudioManager.FLAG_SHOW_UI);
        }

    }
}