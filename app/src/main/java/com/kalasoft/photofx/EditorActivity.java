package com.kalasoft.photofx;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.app.NativeActivity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.ColorDrawable;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.PopupWindow;
import android.widget.TextView;

import org.xmlpull.v1.XmlPullParser;

public class EditorActivity extends NativeActivity implements ActivityCompat.OnRequestPermissionsResultCallback {
    private PopupWindow popupWindow;
    static int i = 0;
    float width, height;
    private int uiShaderProgId;
    private static final int PERMISSION_REQUEST_STORAGE = 0;///DO PERMISSION WHEN NEEDED

    // static {System.loadLibrary("main");}
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    float[] getDisplayParams() {
        float[] displayParams = new float[8];
        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getRealMetrics(displayMetrics);
        displayParams[0] = displayMetrics.widthPixels;
        displayParams[1] = displayMetrics.heightPixels;
        displayParams[2] = displayMetrics.density;
        displayParams[3] = displayMetrics.densityDpi;
        displayParams[4] = displayMetrics.DENSITY_DEVICE_STABLE;
        displayParams[5] = displayMetrics.scaledDensity;
        displayParams[6] = displayMetrics.xdpi;
        displayParams[7] = displayMetrics.ydpi;
        width = displayMetrics.widthPixels;//////////////////just for testing remove these;
        height = displayMetrics.heightPixels;
        return displayParams;

    }

    Bitmap getBitmap(int id) {
        int bitmapId = 0;
        Log.e("getBitmap", "the id is " + id);
        switch (id) {
            case 0:
                bitmapId = R.drawable.bottle;
                break;
            case 1:
                // bitmapId=R.drawable.ssnare;
                break;
            case 2:
                //bitmapId=R.drawable.stom;
                break;
            case 3:
                bitmapId = R.drawable.bottle;
                break;
            default:
                bitmapId = R.drawable.kala;
        }
        Bitmap bitmap = null;
        try {
            // InputStream stream = getContentResolver().openInputStream(imageUri);
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;//scaling doesnt depend on screenDensity
            bitmap = BitmapFactory.decodeResource(getResources(), bitmapId, options);
        } catch (Exception e) {
            e.printStackTrace();
        }


        return bitmap;
    }

    public void hideSystemUI() {

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
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        });


    }

    public void showSystemUI() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
    }

    private void changeVolume(boolean increase) {
        AudioManager audioManager = (AudioManager) getApplicationContext().getSystemService(getApplicationContext().AUDIO_SERVICE);

        if (increase) {

            Log.e("Java ", "increasevbolume");
            audioManager.adjustVolume(AudioManager.ADJUST_RAISE, AudioManager.FLAG_SHOW_UI);
        } else {
            //decrease volume
            Log.e("Java ", "decreasevbolume");
            audioManager.adjustVolume(AudioManager.ADJUST_LOWER, AudioManager.FLAG_SHOW_UI);
        }

    }

    public void openFileExplorer() {
       // Log.e("FIleExplore", "DFDF");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                try {
                    requestStoragePermission();
                    ChooserView b = new ChooserView(getApplicationContext(), uiShaderProgId);
                    popupWindow = new PopupWindow(b, (int) width, (int) height, true);
                    popupWindow.setFocusable(false);//set true after full screen
                    //popupWindow.setBackgroundDrawable(new ColorDrawable());
                    b.popupWindow = popupWindow;
                    popupWindow.showAtLocation(getWindow().getDecorView(), Gravity.TOP, 0, 0);
                    popupWindow.update(-1200, -1200, -1, -1, true);
                    try {


                        b.setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    popupWindow.setFocusable(true);//set true after full screen
                    popupWindow.update(-500, -300, -1, -1, true);

                    popupWindow.setOnDismissListener(new PopupWindow.OnDismissListener() {
                        @Override
                        public void onDismiss() {
                            hideSystemUI();
                            Log.e("FileChooser", "closed");
                        }
                    });
                /*  b.setOnTouchListener(new View.OnTouchListener() {///set inside ChooserView
                      @Override
                      public boolean onTouch(View v, MotionEvent event) {
                          Log.e("filechooserTouched","touched "+ " java progr id is" + uiShaderProgId);
                      //    InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
                        //  imm.showSoftInput(getWindow().getDecorView(), 0);
                          return false;
                      }
                  });*/

                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        });
    }

    public void setUiShaderProgramId(int progId) {
        uiShaderProgId = progId;
        return;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        // BEGIN_INCLUDE(onRequestPermissionsResult)
        if (requestCode == PERMISSION_REQUEST_STORAGE) {
            // Request for camera permission.
            if (grantResults.length == 1 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // Permission has been granted. Start camera preview Activity.
                Log.e("Permission", "Granted");
                //startCamera();
            } else {
                // Permission request was denied.

            }
        }


    }

    public void requestStoragePermission()
    {
        // Check if the  permission has been granted
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                == PackageManager.PERMISSION_GRANTED)
        {
            // Permission is already available, start work else rquest
        }
        else
            {
                //request permision
                // Permission has not been granted and must be requested.
                if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.READ_EXTERNAL_STORAGE))
                {
                    // Provide an additional rationale to the user if the permission was not granted
                    // and the user would benefit from additional context for the use of the permission.
                    // Display a SnackBar with cda button to request the missing permission.
                  /*  Snackbar.make(mLayout, R.string.camera_access_required,
                            Snackbar.LENGTH_INDEFINITE).setAction(R.string.ok, new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            // Request the permission
                            ActivityCompat.requestPermissions(MainActivity.this,
                                    new String[]{Manifest.permission.CAMERA},
                                    PERMISSION_REQUEST_CAMERA);
                        }
                    }).show();*/

                } else {
                  //  Snackbar.make(mLayout, R.string.camera_unavailable, Snackbar.LENGTH_SHORT).show();
                    // Request the permission. The result will be received in onRequestPermissionResult().
                    ActivityCompat.requestPermissions(this,
                            new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, PERMISSION_REQUEST_STORAGE);
                }
            }
    }
}
