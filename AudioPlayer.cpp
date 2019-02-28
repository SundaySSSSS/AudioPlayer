#include "AudioPlayer.h"
#include <stdio.h>
#include <QDebug>

namespace AudioPlayerNS
{

#define MAX_MIX_SIZE (64 * 1024)

void audio_callback(void *udata, Uint8 *stream, int len)
{
    //qDebug() << "audio callback" << len;
    if (udata == NULL)
    {
        qDebug() << "invalid udata";
        return;
    }
    //取出音频播放器指针
    AudioPlayer* pAp = (AudioPlayer*)udata;

    SDL_memset(stream, 0, len);

    //获取当前存放的数据量
    int32 dataSize = pAp->m_dataQueue.getUsedSize();
    if (dataSize <= 0)
    {   //当前已经没有数据了, 直接退出
        //qDebug() << "no data";
        pAp->m_isNoData = true;
        return;
    }
    else
    {
        pAp->m_isNoData = false;
    }
    if (len > dataSize)
    {
        len = dataSize;
    }
    //限制len的大小
    if (len > MAX_MIX_SIZE)
    {
        len = MAX_MIX_SIZE;
    }
    //从数据队列中读取数据
    memset(pAp->m_pTempBuffer, 0, MAX_MIX_SIZE);
    pAp->popData(pAp->m_pTempBuffer, len);

    int volume = pAp->m_volume;
    //qDebug() << "mix len =" << len;
    SDL_MixAudio(stream, (unsigned char*)pAp->m_pTempBuffer, len, volume);
    pAp->updatePlayedLen(len);
}

AudioPlayer::AudioPlayer()
{
    m_pTempBuffer = new char[MAX_MIX_SIZE];
    m_volume = 128;
    m_isNoData = false;
}

AudioPlayer::~AudioPlayer()
{
    if (m_pTempBuffer != NULL)
    {
        delete[] m_pTempBuffer;
        m_pTempBuffer = NULL;
    }
}

APRet AudioPlayer::init(AudioPlayerNS::AudioInfo info)
{
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
    {   //SDL尚未进行初始化， 初始化
        if (SDL_Init(SDL_INIT_AUDIO))
        {   //没有初始化成功
            return AP_SDL_INIT_ERR;
        }
    }

    //设置音频信息
    SDL_AudioSpec audioSpec;
    audioSpec.freq = info.fs;
    audioSpec.format = getAudioFormatFromDataFormat(info.format);
    audioSpec.channels = info.channels;
    audioSpec.silence = 0;
    audioSpec.samples = 1024;
    audioSpec.callback = audio_callback;
    audioSpec.userdata = this;
    if (SDL_OpenAudio(&audioSpec, NULL) < 0)
    {
        return AP_SDL_OPEN_ERR;
    }
    else
    {
        m_dataQueue.clear();
        m_playedLen = 0;
        return AP_OK;
    }

}

void AudioPlayer::play()
{
    SDL_PauseAudio(0);
}

void AudioPlayer::pause()
{
    AudioState state = getState();
    if (state == AUDIO_PLAYING)
    {
        SDL_PauseAudio(1);
    }
}

void AudioPlayer::resume()
{
    AudioState state = getState();
    if (state == AUDIO_PAUSED)
    {
        SDL_PauseAudio(0);
    }
}

void AudioPlayer::destroy()
{
    SDL_CloseAudio();
    SDL_Quit();
}

void AudioPlayer::setVolume(int32 volume)
{
    if (volume < 0)
    {
        volume = 0;
    }
    else if (volume > 128)
    {
        volume = 128;
    }

    m_volume = volume;
}

APRet AudioPlayer::pushData(const char *data, int32 len)
{
    m_mutex.lock();
    APRet ret = AP_OK;
    int32 freeSpace = m_dataQueue.getFreeSize();
    if (freeSpace >= len)
    {
        if (m_dataQueue.write(data, len) != len)
        {
            ret = AP_UNKNOWN_ERR;
        }
    }
    else
    {
        ret = AP_BUFFER_FULL;
    }
    m_mutex.unlock();
    return ret;
}

AudioState AudioPlayer::getState()
{
    SDL_AudioStatus status = SDL_GetAudioStatus();
    AudioState ret = AUDIO_PLAYING;
    switch (status)
    {
    case SDL_AUDIO_STOPPED:
        ret = AUDIO_STOPPED;
        break;
    case SDL_AUDIO_PAUSED:
        ret = AUDIO_PAUSED;
        break;
    case SDL_AUDIO_PLAYING:
    default:
        ret = AUDIO_PLAYING;
        break;
    }
    return ret;
}

void AudioPlayer::updatePlayedLen(int64 len)
{
    m_playedLen += len;
}

APRet AudioPlayer::popData(char *data, int32 &len)
{
    m_mutex.lock();
    APRet ret = AP_OK;
    int32 usedSpace = m_dataQueue.getUsedSize();
    if (usedSpace >= len)
    {
        if (m_dataQueue.read(data, len) != len)
        {
            ret = AP_UNKNOWN_ERR;
        }
    }
    else
    {
        ret = AP_BUFFER_DATA_NOT_ENOUGH;
    }
    m_mutex.unlock();
    return ret;
}

SDL_AudioFormat AudioPlayer::getAudioFormatFromDataFormat(DataFormat dataFormat)
{
    SDL_AudioFormat sdl_audioFormat = AUDIO_S16;
    switch (dataFormat)
    {
    case FORMAT_FLOAT32:
        sdl_audioFormat = AUDIO_F32;
        break;
    case FORMAT_INT32:
    case FORMAT_UINT32: //sdl播放器不支持32位无符号整数, 后期需进行转换
        sdl_audioFormat = AUDIO_S32;
        break;
    case FORMAT_INT16:
        sdl_audioFormat = AUDIO_S16;
        break;
    case FORMAT_UINT16:
        sdl_audioFormat = AUDIO_U16;
        break;
    case FORMAT_INT8:
        sdl_audioFormat = AUDIO_S8;
        break;
    case FORMAT_UINT8:
        sdl_audioFormat = AUDIO_U8;
        break;
    default:
        //错误，使用默认值
        break;
    }
    return sdl_audioFormat;
}

}
