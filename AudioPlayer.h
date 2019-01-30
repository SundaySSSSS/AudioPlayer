#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

extern "C"
{
#include "include/sdl/SDL.h"
}
#include "typedef.h"

namespace AudioPlayerNS
{

typedef enum _DataFormat
{
    FORMAT_FLOAT,
    FORMAT_INT32,
    FORMAT_INT16,
    FORMAT_UINT16,
    FORMAT_INT8,
    FORMAT_UINT8,
}
DataFormat;

typedef enum _AudioPlayerRet
{
    AP_OK = 0,
    AP_SDL_INIT_ERR,    //SDL底层库初始化失败
    AP_SDL_OPEN_ERR,    //SDL底层库打开文件失败
    AP_BUFFER_FULL, //数据buffer满
    AP_UNKNOWN_ERR, //未知错误
}
APRet;

typedef struct _AudioInfo
{
    int32 fs;   //音频采样率
    DataFormat format; //数据格式
    uint8 channels; //通道数   1-单通道 2-双通道
}
AudioInfo;

class AudioPlayer
{
public:
    AudioPlayer();

    APRet init(AudioInfo info);
    APRet play();   //播放
    APRet pause();  //暂停播放
    APRet destroy();   //停止播放， 调用此接口后， 需要重新进行init
    APRet pushData(char* data, int32 len);  //向缓存中输入数据， 如果已经播完， 则暂停，如果重新输入数据， 则自动重新开始播放
    int playWav(const char* filePath);

private:
    SDL_AudioFormat getAudioFormatFromDataFormat(DataFormat dataFormat);
};

}

#endif // AUDIOPLAYER_H
