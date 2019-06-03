#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include "gif_lib.h"
#include <android/log.h>
#define  LOG_TAG    "wangyi"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define argb(a,r,g,b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)
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
    GifFileType *gifFileType = DGifOpenFileName(path, &error);
    DGifSlurp(gifFileType); //初始化D

    GifBean *gifBean = static_cast<GifBean *>(malloc(sizeof(GifBean)));
    memset(gifBean,0, sizeof(GifBean));   //清空申请到的内存空间
    gifFileType->UserData = gifBean;
    //申请延时时间数组并将内存空间初始化
    gifBean->dealys = (int*)(malloc(sizeof(int) * gifFileType->ImageCount));
    memset(gifBean->dealys,0,sizeof(int) * gifFileType->ImageCount);


    gifBean->current_frame = 0;
    gifBean->total_frame = gifFileType->ImageCount;


    ExtensionBlock* ext;
    for(int i= 0;i < gifFileType->ImageCount; i++) {
       SavedImage image = gifFileType->SavedImages[i];
       for(int j =0;j < image.ExtensionBlockCount; j++) {
           ExtensionBlock block =  image.ExtensionBlocks[j];
           if(block.Function == GRAPHICS_EXT_FUNC_CODE) {
               ext = &block;
               break;
           }

       }
       if (ext) {
           //单位是ms
           int frame_delay =10*(ext->Bytes[1] | (ext->Bytes[2] << 8));//ms
           gifBean->dealys[i] = frame_delay;
           LOGE("时间  %d   ",frame_delay);
       }
    }
    LOGE("gif  长度大小    %d  ",gifFileType->ImageCount);
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

}

//绘制bitmap
void drawBitmap(GifFileType *pType, GifBean *pBean, void * pixels, AndroidBitmapInfo info) {
//  当前帧
    SavedImage savedImage = pType->SavedImages[pBean->current_frame];
    //整幅图片的首地址
    int* px = (int *)pixels;
    int  pointPixel;
    GifImageDesc frameInfo = savedImage.ImageDesc;
    GifByteType  gifByteType;//压缩数据
//    rgb数据     压缩工具
    ColorMapObject* colorMapObject=frameInfo.ColorMap;
//    Bitmap 往下便宜
    px = (int *) ((char*)px + info.stride * frameInfo.Top);
    //    每一行的首地址
    int *line;
    for (int y = frameInfo.Top; y <frameInfo.Top+frameInfo.Height ; ++y) {
        line=px;
        for (int x = frameInfo.Left; x <frameInfo.Left + frameInfo.Width ; ++x) {
//            拿到每一个坐标的位置  索引    ---》  数据
            pointPixel=  (y-frameInfo.Top)*frameInfo.Width+(x-frameInfo.Left);
//            索引   rgb   LZW压缩  字典   （）缓存在一个字典
//解压
            gifByteType= savedImage.RasterBits[pointPixel];
            GifColorType gifColorType=colorMapObject->Colors[gifByteType];
            line[x]=argb(255,gifColorType.Red, gifColorType.Green, gifColorType.Blue);
        }
        px = (int *) ((char*)px + info.stride);
    }

}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_zhangzd_cusgifplayer_GifHandler_updateFrame(
        JNIEnv *env, jobject instance, jlong ndkGif, jobject bitmap) {
    //获取GIF的每一帧，然后将每一帧绘制到bitmap上，绘制结束后，返回延迟时间

    GifFileType *fileType =(GifFileType*)ndkGif;
    GifBean *gifBean = (GifBean*)fileType->UserData;

    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env,bitmap,&info);  //获取bitmap的相关信息

    void *pixcels;
    //锁定bitmap,对bitmap 进行绘制，然后解锁
    AndroidBitmap_lockPixels(env, bitmap, &pixcels);
    drawBitmap(fileType, gifBean, bitmap, info);

    //绘制bitmap结束后，当前帧加一
    gifBean->current_frame ++;
    //判断若超过最大帧数，则将当前帧置0
    if(gifBean->current_frame >= gifBean->total_frame -1) {
        gifBean->current_frame = 0;
        LOGE("重新过来  %d  ",gifBean->current_frame);
    }
    AndroidBitmap_unlockPixels(env,bitmap);
    return gifBean->dealys[gifBean->current_frame];

}