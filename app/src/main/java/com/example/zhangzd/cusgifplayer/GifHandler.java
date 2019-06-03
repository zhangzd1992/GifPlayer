package com.example.zhangzd.cusgifplayer;

import android.graphics.Bitmap;

/**
 * @Description:
 * @Author: zhangzd
 * @CreateDate: 2019-06-03 10:48
 */
public class GifHandler {
    static {
        System.loadLibrary("native-lib");
    }
    public long gifAddr;   //git 的内存地址
    public GifHandler(String path) {
        this.gifAddr = loadPath(path);
    }

    public int getWidth() {
        return getWidth(gifAddr);
    }

    public int getHeight() {
        return getHeight(gifAddr);
    }


    public int updateFrame (Bitmap bitmap) {
        return updateFrame(gifAddr,bitmap);
    }



    public native long loadPath(String path);
    public native int getWidth(long ndkGif);
    public native int getHeight(long ndkGif);
    //    隔一段事件 调用一次
    public native int updateFrame(long ndkGif, Bitmap bitmap);
}
