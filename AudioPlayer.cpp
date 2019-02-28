#include "AudioPlayer.h"
#include <stdio.h>
#include <QDebug>

namespace AudioPlayerNS
{

static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;

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
        qDebug() << "no data";
        return;
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

    //qDebug() << "mix len =" << len;
    SDL_MixAudio(stream, (unsigned char*)pAp->m_pTempBuffer, len, SDL_MIX_MAXVOLUME);
}

AudioPlayer::AudioPlayer()
{
    m_pTempBuffer = new char[MAX_MIX_SIZE];
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
        return AP_OK;
    }

}

void AudioPlayer::play()
{
    SDL_PauseAudio(0);
}

void AudioPlayer::destroy()
{
    SDL_CloseAudio();
    SDL_Quit();
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

int AudioPlayer::playWav(const char *filePath)
{
    //Init
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        qDebug() << QString().sprintf("Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }
    //SDL_AudioSpec
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = 44100;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = 2;
    wanted_spec.silence = 0;
    wanted_spec.samples = 1024;
    //wanted_spec.callback = fill_audio;

    if (SDL_OpenAudio(&wanted_spec, NULL) < 0)
    {
        qDebug() << QString().sprintf("can't open audio.\n");
        return -1;
    }

    FILE *fp = fopen(filePath, "rb+");
    if(fp==NULL)
    {
        qDebug() << QString().sprintf("cannot open this file\n");
        return -1;
    }

    int pcm_buffer_size = 64 * 1024;
    char *pcm_buffer=(char *)malloc(pcm_buffer_size);
    int data_count=0;

    while(1)
    {
        if (fread(pcm_buffer, 1, pcm_buffer_size, fp) != pcm_buffer_size)
        {
            // Loop
            fseek(fp, 0, SEEK_SET);
            fread(pcm_buffer, 1, pcm_buffer_size, fp);
            data_count=0;
        }
        qDebug() << QString().sprintf("Now Playing %10d Bytes data.\n",data_count);
        data_count+=pcm_buffer_size;
        //Set audio buffer (PCM data)
        audio_chunk = (Uint8 *) pcm_buffer;
        //Audio buffer length
        audio_len = pcm_buffer_size;
        audio_pos = audio_chunk;
        //Play
        SDL_PauseAudio(0);
        while(audio_len > 0)//Wait until finish
            SDL_Delay(1);
    }
    return 0;
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
