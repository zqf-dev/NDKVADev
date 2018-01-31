package com.zqf.ndkaudiovideodemo;

/**
 * class from
 * Created by zqf
 * Time 2018/1/30 16:05
 */

public class FFmpegUtils {

    //测试FFmpeg配置环境
    public static native void cppTestFFmpegConfig();

    //NDK音视频编解码：FFmpeg-视频解码-视频像素数据(YUV420P)
    public static native void cppFFmpegVideoDecode(String inFilePath, String outFilePath);
}
