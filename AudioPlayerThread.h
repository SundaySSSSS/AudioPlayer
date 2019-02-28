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

typedef struct _AudioMemInfo
{
    int32 fs;   //采样率
    ChannelType channeType; //通道类型
    DataFormat dataFormat;  //数据类型
}
AudioMemInfo;

class AudioPlayerThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioPlayerThread(QObject* parent = NULL);

    //初始化(文件模式: 从文件中读取数据)
    bool init(const AudioFileInfo& fileInfo);

    //初始化为内存模式, 等待使用push来输入数据播放
    bool init(const AudioMemInfo& memInfo);
    bool push(const char* data, int32 len);

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
    bool m_isMemMode;
    AudioFileInfo m_fileInfo;
    AudioMemInfo m_memInfo;
    AudioPlayer m_audioPlayer;

    virtual void run();

    //文件模式的线程函数
    void fileModeRun();
    //内存模式的线程函数
    void memModeRun();

    float m_playedRate;
    //检查文件进度, 适时发送进度信号
    //返回值: true-已经播放完毕 false-尚未播放完毕
    bool checkFileProcess();

signals:
    void sendPlayFinished();    //发送播放完毕
    void sendPlayFileProcess(float rate);   //发送文件播放比例
};


}   //namespace AudioPlayerNS

#endif // AUDIOPLAYERTHREAD_H
