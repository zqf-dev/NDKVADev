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

}


JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppTestFFmpegConfig(JNIEnv* env,jobject jobj) {

        const char* config= avcodec_configuration();
        __android_log_print(ANDROID_LOG_INFO,"main","配置信息：%s",config);
}
