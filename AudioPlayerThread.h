#ifndef AUDIOPLAYERTHREAD_H
#define AUDIOPLAYERTHREAD_H

#include <QThread>
#include <QFileInfo>
#include "typedef.h"
#include "AudioPlayer.h"

namespace AudioPlayerNS
{

typedef struct _AudioFileInfo
{
    QString path;   //文件路径
    int32 fs;   //采样率
    ChannelType channeType; //通道类型
    DataFormat dataFormat;  //数据类型
    int64 startReadPos; //从文件的哪里开始读取(bytes)
    int64 stopReadPos;  //从文件的哪里结束读取(bytes)
}
AudioFileInfo;

/* 音频播放状态枚举 */
typedef enum _AudioState
{
    AUDIO_UNINITED,  //未初始化状态
    AUDIO_READY,    //就绪状态
    AUDIO_PLAYING,  //播放状态
    AUDIO_PAUSE,    //暂停状态
}
AudioState;

/*
状态迁移过程
创建 -> AUDIO_UNINITED -> setPlayFile() -> AUDIO_READY-> play() -> AUDIO_PLAYING
AUDIO_PLAYING -> pause() -> AUDIO_PAUSE -> resume() -> AUDIO_PLAYING
AUDIO_PLAYING -> stop() -> AUDIO_READY
AUDIO_PAUSE -> stop() -> AUDIO_READY
任何状态 -> clear() -> AUDIO_UNINITED
*/

class AudioPlayerThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioPlayerThread(QObject* parent = NULL);

    //初始化
    bool init(const AudioFileInfo& fileInfo);
    //播放
    bool play();
    //暂停
    void pause();
    //恢复
    void resume();
    //停止
    void stop();
    //设置音量
    void setVolume(int volume);
    //清空
    void clear();
private:
    AudioFileInfo m_fileInfo;
    QMutex m_mutex;
    AudioState m_state; //标记当前播放状态
    AudioState getState();
    void setState(AudioState state);
    virtual void run();
};

}   //namespace AudioPlayerNS

#endif // AUDIOPLAYERTHREAD_H
