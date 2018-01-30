#include <jni.h>
#include <string>
//导入FFmpeg头文件->类似于Java导包
#include <android/log.h>

//注意：extern "C"{}->表示C++开发支持C开发（规范）
extern "C"{
//核心库
#include "libavcodec/avcodec.h"
//封装格式处理库
#include "libavformat/avformat.h"
//工具库
#include "libavutil/imgutils.h"
//视频像素数据格式库
#include "libswscale/swscale.h"
//音频采样数据
#include "libswresample/swresample.h"

//1.NDK音视频编解码：FFmpeg-测试配置
//声明函数
//测试环境
JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppTestFFmpegConfig(JNIEnv *env, jobject jobj);

//2.NDK音视频编解码：FFmpeg-视频解码-视频像素数据(YUV420P)
JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppFFmpegVideoDecode(
                    JNIEnv *env, jobject jobj,jstring infilepath,jstring outfilepath);

}


JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppTestFFmpegConfig(JNIEnv* env,jobject jobj) {

        const char* config= avcodec_configuration();
        __android_log_print(ANDROID_LOG_INFO,"main","配置信息：%s",config);
}

//2.NDK音视频编解码：FFmpeg-视频解码-视频像素数据(YUV420P)
JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppFFmpegVideoDecode(
                    JNIEnv *env, jobject jobj,jstring infilepath,jstring outfilepath){

        //第一步 注册组件
        //例如：视频解码器、音频解码器、视频编码器、音频编码器、封装格式等…
        av_register_all();

}
