#include "AudioPlayer.h"
#include <stdio.h>
#include <QDebug>

namespace AudioPlayerNS
{

static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;


void audio_callback(void *udata, Uint8 *stream, int len)
{
    qDebug() << "audio callback" << len;
}

void  fill_audio(void *udata,Uint8 *stream,int len){
    //SDL 2.0
    SDL_memset(stream, 0, len);
    if(audio_len==0)		/*  Only  play  if  we  have  data  left  */
        return;
    len=(len>audio_len?audio_len:len);	/*  Mix  as  much  data  as  possible  */

    SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

AudioPlayer::AudioPlayer()
{
    ;
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
    wanted_spec.callback = fill_audio;

    if (SDL_OpenAudio(&wanted_spec, NULL)<0)
    {
        qDebug() << QString().sprintf("can't open audio.\n");
        return -1;
    }

    //FILE *fp=fopen("../NocturneNo2inEflat_44.1k_s16le.pcm","rb+");
    FILE *fp = fopen(filePath, "rb+");
    if(fp==NULL){
        qDebug() << QString().sprintf("cannot open this file\n");
        return -1;
    }
    //For YUV420P
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
    SDL_AudioFormat sdl_audioFormat = AUDIO_S16SYS;
    switch (dataFormat)
    {
    case FORMAT_FLOAT:
        sdl_audioFormat = AUDIO_F32;
        break;
    case FORMAT_INT32:
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

APRet AudioPlayer::init(AudioPlayerNS::AudioInfo info)
{
    if (SDL_Init(SDL_INIT_AUDIO))
    {
        return AP_SDL_INIT_ERR;
    }
    else
    {
        SDL_AudioSpec audioSpec;
        audioSpec.freq = info.fs;
        audioSpec.format = getAudioFormatFromDataFormat(info.format);
        audioSpec.channels = info.channels;
        audioSpec.silence = 0;
        audioSpec.samples = 1024;
        audioSpec.callback = audio_callback;
        if (SDL_OpenAudio(&audioSpec, NULL) < 0)
        {
            return AP_SDL_OPEN_ERR;
        }
        else
        {
            return AP_OK;
        }
    }
}

APRet AudioPlayer::play()
{
    SDL_PauseAudio(0);
    return AP_OK;
}

APRet AudioPlayer::pushData(char *data, int32 len)
{

}

}