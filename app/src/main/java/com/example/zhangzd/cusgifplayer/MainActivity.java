package com.example.zhangzd.cusgifplayer;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    GifHandler gifHandler;
    Bitmap bitmap;
    ImageView ivGIf;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btStart = findViewById(R.id.btStart);
        ivGIf = findViewById(R.id.ivGif);
        btStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                loadGif();
            }
        });
    }

    @SuppressLint("HandlerLeak")
    Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            int updateFrame = gifHandler.updateFrame(bitmap);
            handler.sendEmptyMessageAtTime(1,updateFrame);
            ivGIf.setImageBitmap(bitmap);


        }
    };

    //加载GIF
    private void loadGif() {
        File file = new File(Environment.getExternalStorageDirectory(), "demo.gif");
        gifHandler = new GifHandler(file.getAbsolutePath());
        bitmap = Bitmap.createBitmap(gifHandler.getWidth(), gifHandler.getHeight(), Bitmap.Config.RGB_565);
        int nextFrame = gifHandler.updateFrame(bitmap);
        handler.sendEmptyMessageAtTime(1,nextFrame);

    }
}
