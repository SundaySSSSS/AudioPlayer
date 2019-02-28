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

class AudioPlayerThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioPlayerThread(QObject* parent = NULL);

    //初始化
    bool init(const AudioFileInfo& fileInfo);
    //播放
    void play();
    //暂停
    void pause();
    //恢复
    void resume();
    //停止
    void stop();
    //设置音量
    void setVolume(int volume);
private:
    bool m_isRunning;
    AudioFileInfo m_fileInfo;
    AudioPlayer m_audioPlayer;

    virtual void run();

signals:
    void sendPlayFinished();    //发送播放完毕
    void sendPlayFileProcess(float rate);   //发送文件播放比例
};

}   //namespace AudioPlayerNS

#endif // AUDIOPLAYERTHREAD_H
