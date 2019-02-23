#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

extern "C"
{
#include "include/sdl/SDL.h"
}
#include "typedef.h"
#include <string>
#include <queue>

namespace AudioPlayerNS
{

#define MAX_MEM_COST (64 * 1024 * 1024) //允许的内存消耗

//内存片结构体
typedef struct _MemPiece
{
    char* data;
    int32 len;
}
MemPiece;

//数据队列, 内部存储的是一片片内存片.
class DataQueue
{
public:
    DataQueue(int32 maxLen = MAX_MEM_COST)
    {
        m_maxLen = maxLen;
        m_dataSize = 0;
    }
    ~DataQueue()
    {
        //销毁所有内存空间
        while (m_queue.size() > 0)
        {
            MemPiece mp = m_queue.front();
            m_queue.pop();
            delete[] mp.data;
            mp.data = NULL;
        }
    }

    //向数据队列中加入一个内存片
    bool push(const char* data, int32 len)
    {
        bool ret = true;
        if (m_dataSize + len <= m_maxLen)
        {   //允许存放此数据
            MemPiece mp;
            mp.data = new char[len];
            memcpy(mp.data, data, len);
            mp.len = len;
            m_queue.push(mp);
            m_dataSize += len;
        }
        else
        {
            ret = false;
        }
        return ret;
    }

    //获取队列头的内存片(取出, 但不会删除)
    bool getFront(char** data, int32& len)
    {
        bool ret = true;
        if (m_queue.size() > 0)
        {
            MemPiece mp = m_queue.front();
            *data = mp.data;
            len = mp.len;
        }
        else
        {
            ret = false;
        }
        return ret;
    }

    //删除队列头的内存片
    void pop()
    {
        if (m_queue.size() > 0)
        {
            MemPiece mp = m_queue.front();
            m_queue.pop();
            delete[] mp.data;
            mp.data = NULL;
            m_dataSize -= mp.len;
        }
    }

private:
    int32 m_dataSize;   //当前数据占用堆空间的大小
    int32 m_maxLen;     //当前数据的最大长度
    std::queue<MemPiece> m_queue; //数据队列
};

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
    std::string filePath;   //文件路径
}
AudioInfo;

//音频回调函数
void audio_callback(void *udata, Uint8 *stream, int len);

class AudioPlayer
{
public:
    AudioPlayer();

    APRet init(AudioInfo info);
    APRet play();   //播放
    APRet pause();  //暂停播放
    APRet destroy();   //停止播放， 调用此接口后， 需要重新进行init
    APRet pushData(const char* data, int32 len);  //向缓存中输入数据， 如果已经播完， 则暂停，如果重新输入数据， 则自动重新开始播放

    int playWav(const char* filePath);

    //将音频回调函数设置为友元
    friend void audio_callback(void *udata, Uint8 *stream, int len);

private:
    DataQueue m_dataQueue;

    SDL_AudioFormat getAudioFormatFromDataFormat(DataFormat dataFormat);
};

}

#endif // AUDIOPLAYER_H
