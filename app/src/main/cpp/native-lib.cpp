#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include "gif_lib.h"


//保存GIF相关信息
typedef struct GifBean{
    //当前播放的帧数
    int current_frame;
    //总帧数
    int total_frame;
    //延迟时间数组
    int *dealys;
} GifBean;



extern "C" JNIEXPORT jstring JNICALL
Java_com_example_zhangzd_cusgifplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_zhangzd_cusgifplayer_GifHandler_loadPath(JNIEnv *env, jobject instance,
                                                          jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int error;
    //获取GIF内存地址
    GifFileType *gifFileType = EGifOpenFileName(path, false, &error);
    DGifSlurp(gifFileType); //初始化

    GifBean *gifBean = static_cast<GifBean *>(malloc(sizeof(GifBean)));
    memset(gifBean,0, sizeof(GifBean));   //清空申请到的内存空间
    gifFileType->UserData = gifBean;
    gifBean->current_frame = 0;
    gifBean->total_frame = gifFileType->ImageCount;
    //申请延时时间数组并将内存空间初始化
    gifBean->dealys = static_cast<int *>(malloc(sizeof(int) * gifFileType->ImageCount));
    memset(gifBean->dealys,0,sizeof(int) * gifFileType->ImageCount);


    for(int i= 0;i < gifFileType->ImageCount; i++) {
       SavedImage image = gifFileType->SavedImages[i];
       for(int j =0;j < image.ExtensionBlockCount; j++) {
           ExtensionBlock block =  image.ExtensionBlocks[j];
           if(block.Function == GRAPHICS_EXT_FUNC_CODE) {
               //单位是ms
               gifBean->dealys[i] = (block.Bytes[1] | block.Bytes[2] << 8) * 10;
               break;
           }

       }
    }





    env->ReleaseStringUTFChars(path_, path);
    return (jlong) gifFileType;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_zhangzd_cusgifplayer_GifHandler_getWidth__J(JNIEnv *env, jobject instance,
                                                             jlong ndkGif) {

    GifFileType *gifFileType = reinterpret_cast<GifFileType *>(ndkGif);
    return gifFileType->SWidth;

}


extern "C" JNIEXPORT jint JNICALL
Java_com_example_zhangzd_cusgifplayer_GifHandler_getHeight(JNIEnv *env, jobject instance,
                                                              jlong ndkGif) {
    GifFileType *gifFileType = reinterpret_cast<GifFileType *>(ndkGif);
    return gifFileType->SHeight;

}extern "C" JNIEXPORT jint JNICALL
Java_com_example_zhangzd_cusgifplayer_GifHandler_updateFrame(
        JNIEnv *env, jobject instance, jlong ndkGif, jobject bitmap) {
    //获取GIF的每一帧，然后将每一帧绘制到bitmap上，绘制结束后，返回延迟时间

    GifFileType *fileType =(GifFileType*)ndkGif;
    GifBean *gifBean = (GifBean*)fileType->UserData;

    AndroidBitmapInfo* info;
    AndroidBitmap_getInfo(env,bitmap,info);  //获取bitmap的相关信息

    //获取到当前帧
    SavedImage currFrame = fileType->SavedImages[gifBean->current_frame];


}