#include <jni.h>
#include <string>
//导入FFmpeg头文件->类似于Java导包
#include <android/log.h>

//注意：extern "C"{}->表示C++开发支持C开发（规范）
extern "C" {
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
JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppTestFFmpegConfig(JNIEnv *env,
                                                                                      jobject jobj);

//2.NDK音视频编解码：FFmpeg-视频解码-视频像素数据(YUV420P)
JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppFFmpegVideoDecode(
        JNIEnv *env, jobject jobj, jstring infilepath, jstring outfilepath);

}


JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppTestFFmpegConfig(JNIEnv *env,
                                                                                      jobject jobj) {

    const char *config = avcodec_configuration();
    __android_log_print(ANDROID_LOG_INFO, "Tag", "配置信息：%s", config);
}

//2.NDK音视频编解码：FFmpeg-视频解码-视频像素数据(YUV420P)
JNIEXPORT void JNICALL Java_com_zqf_ndkaudiovideodemo_FFmpegUtils_cppFFmpegVideoDecode(
        JNIEnv *env, jobject jobj, jstring jinfilepath, jstring joutfilepath) {

    //第一步 注册组件
    //例如：视频解码器、音频解码器、视频编码器、音频编码器、封装格式等…
    av_register_all();
    //第二步：打开封装格式->解封装
    //例如：mp4、mov、avi等等...
    //解除封装->视频压缩数据->h264视频压缩包
    AVFormatContext *avFormatContext = avformat_alloc_context();
    const char *cinFilePath = env->GetStringUTFChars(jinfilepath, NULL);
    int open_result = avformat_open_input(&avFormatContext, cinFilePath, NULL, NULL);
    if (open_result != 0) {
        __android_log_print(ANDROID_LOG_INFO, "Tag", "解封装失败");
        return;
    }
    //第三步：查找流类型
    //流：视频流、音频流、字幕流…
    //不同流解码方式不一样，存在差异
    int find_result = avformat_find_stream_info(avFormatContext, NULL);
    if (find_result < 0) {
        __android_log_print(ANDROID_LOG_INFO, "Tag", "查找流类型失败");
        return;
    }

    //第四步：查找解码器
    //条件：根据查找到的流类型，然后根据流的类型查找到指定解码器
    //例如：查找到视频流->查找视频解码器
    //1、查找流索引位置
    int av_stream_index = -1;
    // avFormatContext->nb_streams代表在AVFormatContext.streams中元素的数量。
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        //判断流是否为视频类型
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            av_stream_index = i;
            break;
        }
    }
    //判断是否找到
    if (av_stream_index == -1) {
        __android_log_print(ANDROID_LOG_INFO, "Tag", "没有找到视频流...");
        return;
    }
    //2、根据解码器下标，查找到视频解码器
    //解码器上下文
    AVCodecContext *avCodecContext = avFormatContext->streams[av_stream_index]->codec;
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "Tag", "没有找到视频解码器...");
        return;
    }

    //第五步：打开解码器
    int avcodec_open_result = avcodec_open2(avCodecContext, avCodec, NULL);
    if (avcodec_open_result != 0) {
        __android_log_print(ANDROID_LOG_INFO, "Tag", "打开视频解码器失败...");
        return;
    }

    //打印信息
    __android_log_print(ANDROID_LOG_INFO, "Tag", "解码器名称：%s", avCodec->name);

    //第六步：读取一帧压缩数据（AVPacket）
    //注意：循环读取
    //av_read_frame：读取一帧压缩数据
    //视频压缩数据
    //AVPacket:压缩数据包(音频压缩数据、视频压缩数据)
    //av_malloc动态读取
    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    //视频的像素数据
    AVFrame *avFrame = av_frame_alloc();

    int avcodec_receive_result = -1;

    //第八步：保存文件(为了保存文件类型统一使用yuv420p文件，所以要进行类型转化
    //1、类型转化
    //参数一：输入帧的宽->avframe_in
    //参数二：输入帧的高->avframe_in
    //参数三：输入帧数据格式
    //参数四：输出帧的宽
    //参数五：输出帧的高
    //参数六：输出帧数据格式
    //参数七：视频像素数据格式类型(YUV420P、YUV422P等等...)
    //参数八：字节对齐类型(C/C++结构体大小规则)

    //保证输入和输出宽高一样所以一样。但是输出格式变为yuv420p
    SwsContext *swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                            avCodecContext->pix_fmt, avCodecContext->width,
                                            avCodecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                                            NULL,
                                            NULL, NULL);


    //开辟一块内存空间，但是我们并不知道是什么类型格式的内存空间
    //首先：开辟内存->作为视频像素数据格式
    AVFrame *avframe_out_yuv420p = av_frame_alloc();

    //其次：确定缓冲区大小
    //参数一：视频像素数据格式类型->YUV420P
    //参数二：缓冲区宽
    //参数三：缓冲区高
    //参数四：字节对其方式
    int buffer_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, avCodecContext->width,
                                               avCodecContext->height, 1);
    //开辟一块buffer_size大小内存空间
    uint8_t *out_buffer = (uint8_t *) av_malloc(buffer_size);

    //向avframe_out_yuv420p->填充数据
    //参数一：目标->填充数据->填到那里去->avframe_out_yuv420p
    //参数二：目标->每一行大小
    //参数三：数据源->out_buffer
    //参数四：目标->格式类型
    //参数五：宽
    //参数六：高
    //参数七：字节对其方式
    av_image_fill_arrays(avframe_out_yuv420p->data, avframe_out_yuv420p->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P, avCodecContext->width, avCodecContext->height, 1);

    //默认y、u、v大小
    int y_size = 0, u_size = 0, v_size = 0;

    //保存的文件out_File_yuv
    const char *coutFilePath = env->GetStringUTFChars(joutfilepath, false);
    FILE *out_File_yuv = fopen(coutFilePath, "wb");
    if (out_File_yuv == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "Tag", "文件打开失败...");
        return;
    }

    //实时查看的帧数
    int frameIndex = 1;

    //av_read_frame读取一帧
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        //读取到了数据
        //判断是否是视频流
        if (avPacket->stream_index == av_stream_index) {
            //第七步：解码一帧压缩数据得到一帧像素数据
            //(方法：循环读取全部视频压缩数据)
            //【发送压缩数据包->视频压缩数据】
            avcodec_send_packet(avCodecContext, avPacket);
            //【解码数据包->接收像素数据】
            avcodec_receive_result = avcodec_receive_frame(avCodecContext, avFrame);
            if (avcodec_receive_result == 0) {
                __android_log_print(ANDROID_LOG_INFO, "Tag", "视频解码一帧数据成功...");

                //第八步 保存文件
                //保证视频像素数据->必需是->yuv420p格式(保存文件类型统一使用yuv420p文件)
                //参数一：视频像素数据上下文
                //参数二：输入数据->视频像素数据
                //参数三：输入画面每一行大小
                //参数四：输入画面每一行开始位置(0开始->原点)
                //参数五：输入数据行数
                //参数六：输出数据
                //参数七：输出画面每一行大小
                //例如：yuv422p->yuv420p
                sws_scale(swsContext, (const uint8_t *const *) avFrame->data,
                          avFrame->linesize, 0, avCodecContext->height,
                          avframe_out_yuv420p->data,
                          avframe_out_yuv420p->linesize);

                //2、写入文件
                //2.1 计算y、u、v写入大小
                //在我们的yuv知识：Y表示亮度，UV表示色度（备注：人的肉眼对亮度敏感，对色度不敏感）
                //YUV420P格式比例
                //两个规范（多个比喻）
                //规范一：Y结构表示一个像素点(一个像素点就是一个Y)
                //规范二：四个Y对应一个U和一个V
                //Y : U : V = 4 : 1 : 1
                y_size = avCodecContext->width * avCodecContext->height;
                u_size = y_size / 4;
                v_size = u_size;
                //2.2 写入文件->按照顺序写入文件
                //首先：写入Y->frame->data[0]
                //参数二：表示开始位置->偏移量
                fwrite(avFrame->data[0], 1, y_size, out_File_yuv);
                fwrite(avFrame->data[1], 1, u_size, out_File_yuv);
                fwrite(avFrame->data[2], 1, v_size, out_File_yuv);

                __android_log_print(ANDROID_LOG_INFO, "Tag", "当前解码到了第%d帧", frameIndex);
                frameIndex++;
            }
        }
    }

    //第九步：关闭解码器->解码完成
    av_packet_free(&avPacket);
    fclose(out_File_yuv);
    av_frame_free(&avFrame);
    av_frame_free(&avframe_out_yuv420p);
    free(out_buffer);
    avcodec_close(avCodecContext);
    avformat_free_context(avFormatContext);

}
