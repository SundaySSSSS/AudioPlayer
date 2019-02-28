#include "AudioPlayerThread.h"

using namespace AudioPlayerNS;

#include <QFile>
#include <QDebug>

AudioPlayerThread::AudioPlayerThread(QObject *parent)
    : QThread(parent)
{
    m_state = AUDIO_UNINITED;
}

bool AudioPlayerThread::setPlayFile(const AudioFileInfo &fileInfo)
{
    bool ret = false;
    if (m_state == AUDIO_UNINITED)
    {
        m_fileInfo = fileInfo;
        QFileInfo qFileInfo(m_fileInfo.path);
        if (qFileInfo.exists())
        {
            if (m_fileInfo.stopReadPos < 0)
            {
                m_fileInfo.stopReadPos = qFileInfo.size();
            }
            m_state = AUDIO_READY;
            ret = true;
        }
    }
    return ret;
}

bool AudioPlayerThread::play()
{
    bool ret = false;
    if (m_state == AUDIO_READY)
    {
        m_state = AUDIO_PLAYING;
        this->start();
        ret = true;
    }
    return ret;
}

void AudioPlayerThread::run()
{
    QFile file(m_fileInfo.path);
    if (file.open(QFile::ReadOnly))
    {
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
        AudioPlayer ap;
        ap.init(audioInfo);
        ap.play();
        //逐段读取文件, 送入音频播放对象中
        qDebug() << "begin read file data";
        int32 dataLen = 256 * 1024;
        char* pData = new char[dataLen];
        file.seek(m_fileInfo.startReadPos);
        while (!file.atEnd() && m_state == AUDIO_PLAYING)
        {
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
                while (isPushed != AudioPlayerNS::AP_OK)
                {
                    isPushed = ap.pushData(pData, realReadLen);
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

        delete[] pData;
        pData = NULL;

        file.close();
        while (1)
        {
            msleep(1000);
        }
    }
    else
    {
        qDebug() << "open file error";
    }
}