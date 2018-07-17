package com.zqf.ndkaudiovideodemo;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private String rootPath = Environment.getExternalStorageDirectory().getAbsolutePath();
    private String inFilePath;
    private String outFilePath;
    private TextView mTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initFile();
        initView();
    }

    //加载音、视频文件
    private void initFile() {
        inFilePath = rootPath.concat("/DownLoad/TestNdk.wmv");
        outFilePath = rootPath.concat("/DownLoad/TestNdk.yuv");
        File file = new File(outFilePath);
        if (file.exists()) {
            Log.e("Tag", "存在");
        } else {
            try {
                Log.e("Tag", "不存在");
                file.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void initView() {
        //测试NDK-FFmpeg配置
        findViewById(R.id.test_ndk_config_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FFmpegUtils.cppTestFFmpegConfig();
            }
        });
        //测试wmv格式视频解码为yuv格式
        findViewById(R.id.test_ndk_video_decode_btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FFmpegUtils.cppFFmpegVideoDecode(inFilePath, outFilePath);
            }
        });
    }
}
