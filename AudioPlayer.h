#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

extern "C"
{
#include "include/sdl/SDL.h"
}
#include "typedef.h"
#include "CycleQueue/CycleQueue.h"
#include <string>
#include <queue>

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
    AP_OK = 0,          //成功
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
    std::string filePath;   //文件路径
}
AudioInfo;

//音频回调函数
void audio_callback(void *udata, Uint8 *stream, int len);

class AudioPlayer
{
public:
    AudioPlayer();
    virtual ~AudioPlayer();

    APRet init(AudioInfo info);
    APRet play();   //播放
    APRet pause();  //暂停播放
    APRet destroy();   //停止播放， 调用此接口后， 需要重新进行init
    APRet pushData(const char* data, int32 len);  //向缓存中输入数据， 如果已经播完， 则暂停，如果重新输入数据， 则自动重新开始播放

    int playWav(const char* filePath);

    //将音频回调函数设置为友元
    friend void audio_callback(void *udata, Uint8 *stream, int len);

private:
    CycleQueue m_dataQueue; //数据队列, 用于存放外界输入的数据
    char* m_pTempBuffer;    //数据缓冲, 在SDL Mix之前临时存放数据

    SDL_AudioFormat getAudioFormatFromDataFormat(DataFormat dataFormat);
};

}

#endif // AUDIOPLAYER_H
