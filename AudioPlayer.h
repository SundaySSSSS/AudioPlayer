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
#include <QMutex>

namespace AudioPlayerNS
{

typedef enum _ChannelType
{
    SINGLE_I = 0,   //单通道
    I_Q = 1,        //双通道
}
ChannelType;

typedef enum _DataFormat
{
    FORMAT_INT8,
    FORMAT_UINT8,
    FORMAT_INT16,
    FORMAT_UINT16,
    FORMAT_INT32,
    FORMAT_UINT32,
    FORMAT_FLOAT32,
}
DataFormat;

typedef enum _AudioPlayerRet
{
    AP_OK = 0,          //成功
    AP_SDL_INIT_ERR,    //SDL底层库初始化失败
    AP_SDL_OPEN_ERR,    //SDL底层库打开文件失败
    AP_BUFFER_FULL, //数据buffer满
    AP_BUFFER_DATA_NOT_ENOUGH,  //数据buffer中数据不足
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

typedef enum _AudioState
{
    AUDIO_STOPPED = 0,
    AUDIO_PLAYING,
    AUDIO_PAUSED
}
AudioState;

//音频回调函数
void audio_callback(void *udata, Uint8 *stream, int len);

class AudioPlayer
{
public:
    AudioPlayer();
    virtual ~AudioPlayer();

    APRet init(AudioInfo info);
    void play();   //播放
    void pause();   //暂停
    void resume();  //恢复播放
    void destroy();   //停止播放， 调用此接口后， 需要重新进行init
    //设置音量, 音量范围为0-128, 超过此范围将自动规范到此范围
    void setVolume(int32 volume);
    APRet pushData(const char* data, int32 len);  //向缓存中输入数据

    AudioState getState();//获取音频播放状态
    int64 getPlayedLen() { return m_playedLen; }

    //将音频回调函数设置为友元
    friend void audio_callback(void *udata, Uint8 *stream, int len);

private:
    QMutex m_mutex;
    CycleQueue m_dataQueue; //数据队列, 用于存放外界输入的数据
    char* m_pTempBuffer;    //数据缓冲, 在SDL Mix之前临时存放数据
    int32 m_volume; //音量

    /* 播放进度控制 */
    int64 m_playedLen;    //已经播放的点数
    //将本次播放的数据传入, 在音频播放回调中使用, 输入参数len为本次播放的长度(bytes)
    void updatePlayedLen(int64 len);

    APRet popData(char* data, int32& len);  //从缓存中取出数据
    SDL_AudioFormat getAudioFormatFromDataFormat(DataFormat dataFormat);
};

}

#endif // AUDIOPLAYER_H
