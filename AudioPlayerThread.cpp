#include "AudioPlayerThread.h"

using namespace AudioPlayerNS;

#include <QFile>
#include <QDebug>

AudioPlayerThread::AudioPlayerThread(QObject *parent)
    : QThread(parent)
{
    m_isRunning = false;
    m_isMemMode = false;
}

bool AudioPlayerThread::init(const AudioFileInfo &fileInfo)
{
    bool ret = false;

    m_fileInfo = fileInfo;
    QFileInfo qFileInfo(m_fileInfo.path);
    if (qFileInfo.exists())
    {
        if (m_fileInfo.stopReadPos < 0)
        {
            m_fileInfo.stopReadPos = qFileInfo.size();
        }

        AudioPlayerNS::AudioInfo audioInfo;
        audioInfo.fs = m_fileInfo.fs;
        audioInfo.format = m_fileInfo.dataFormat;
        if (m_fileInfo.channeType == SINGLE_I)
        {
            audioInfo.channels = 1;
        }
        else
        {
            audioInfo.channels = 2;
        }
        APRet initRet = m_audioPlayer.init(audioInfo);
        if (initRet == AP_OK)
        {
            m_playedRate = 0.0f;
            m_isMemMode = false;
            ret = true;
        }
    }
    return ret;
}

bool AudioPlayerThread::init(const AudioMemInfo &memInfo)
{
    bool ret = false;

    m_memInfo = memInfo;
    AudioPlayerNS::AudioInfo audioInfo;
    audioInfo.fs = m_memInfo.fs;
    audioInfo.format = m_memInfo.dataFormat;
    if (m_memInfo.channeType == SINGLE_I)
    {
        audioInfo.channels = 1;
    }
    else
    {
        audioInfo.channels = 2;
    }
    APRet initRet = m_audioPlayer.init(audioInfo);
    if (initRet == AP_OK)
    {
        m_playedRate = 0.0f;
        m_isMemMode = true;
        ret = true;
    }

    return ret;
}

bool AudioPlayerThread::push(const char *data, int32 len)
{
    bool ret = false;
    if (m_isMemMode)
    {
        APRet pushRet = m_audioPlayer.pushData(data, len);
        if (pushRet == AP_OK)
        {
            ret = true;
        }
    }
    return ret;
}

void AudioPlayerThread::play()
{
    this->stop();
    m_isRunning = true;
    this->start();
}

void AudioPlayerThread::pause()
{
    m_audioPlayer.pause();
}

void AudioPlayerThread::resume()
{
    m_audioPlayer.resume();
}

void AudioPlayerThread::stop()
{
    m_isRunning = false;
    this->wait();
}

void AudioPlayerThread::setVolume(int volume)
{
    m_audioPlayer.setVolume(volume);
}

void AudioPlayerThread::run()
{
    if (m_isMemMode)
    {
        memModeRun();
    }
    else
    {
        fileModeRun();
    }
}

bool AudioPlayerThread::checkFileProcess()
{
    int64 playedLen = m_audioPlayer.getPlayedLen();
    int64 totalLen = m_fileInfo.stopReadPos - m_fileInfo.startReadPos;
    float rate = double(playedLen) / double(totalLen);
    if (rate - m_playedRate > 0.01f)
    {
        emit sendPlayFileProcess(rate);
    }
    if (playedLen >= totalLen)
    {   //已经播放完毕
        return true;
    }
    else
    {   //尚未播放完毕
        return false;
    }
}

void AudioPlayerNS::AudioPlayerThread::fileModeRun()
{
    QFile file(m_fileInfo.path);
    if (file.open(QFile::ReadOnly))
    {
        //逐段读取文件, 送入音频播放对象中
        qDebug() << "begin read file data";
        int32 dataLen = 256 * 1024;
        char* pData = new char[dataLen];
        file.seek(m_fileInfo.startReadPos);
        emit sendPlayFileProcess(0.0f);
        m_audioPlayer.play();
        while (!file.atEnd() && m_isRunning)
        {
            AudioState state = m_audioPlayer.getState();
            checkFileProcess();
            if (state == AUDIO_PAUSED)
            {
                msleep(100);
                continue;
            }
            //判定是否还需要继续读取
            if (file.pos() >= m_fileInfo.stopReadPos)
            {
                break;
            }
            //确定要读取的长度
            int32 tryReadLen = dataLen;
            int64 leftSize = m_fileInfo.stopReadPos - file.pos();
            if (leftSize < dataLen)
            {   //文件可读取范围不足
                tryReadLen = leftSize;
            }
            //读取数据
            int realReadLen = file.read(pData, tryReadLen);
            if (realReadLen > 0)
            {
                AudioPlayerNS::APRet isPushed = AudioPlayerNS::AP_UNKNOWN_ERR;
                while (isPushed != AudioPlayerNS::AP_OK && m_isRunning)
                {
                    checkFileProcess();
                    isPushed = m_audioPlayer.pushData(pData, realReadLen);
                    if (isPushed != AudioPlayerNS::AP_OK)
                    {
                        if (AudioPlayerNS::AP_UNKNOWN_ERR == isPushed)
                        {
                            qDebug() << "pushed error";
                        }
                    }
                    usleep(20);
                }
            }
        }

        //文件已经读取完毕, 等待缓冲区清空. 如果是外界命令停止(m_isRunning == false)则立刻停止
        while (!checkFileProcess() && m_isRunning)
        {
            msleep(50);
        }

        delete[] pData;
        pData = NULL;

        file.close();
        m_audioPlayer.destroy();
        emit sendPlayFileProcess(1.0f);
        emit sendPlayFinished();
    }
    else
    {
        qDebug() << "open file error";
    }
    m_isRunning = false;
}

void AudioPlayerThread::memModeRun()
{
    //内存模式, 不需要线程读取文件, 直接空转即可
    m_audioPlayer.play();
    while (m_isRunning)
    {
        if (m_audioPlayer.getIsNoData())
        {   //当缓冲区被清空后, 就认为播放结束
            emit sendPlayFinished();
        }
        msleep(10);
    }
    m_isRunning = false;
}
