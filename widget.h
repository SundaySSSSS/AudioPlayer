#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>
#include <QThread>
#include <QDebug>
#include "AudioPlayer.h"

namespace Ui {
class Widget;
}

class ReadFileThread: public QThread
{
    Q_OBJECT
public:
    ReadFileThread() { m_isRunning = false; }
    void setFilePath(QString filePath) { m_filePath = filePath; }
    void stop() { m_isRunning = false; }
    virtual void run()
    {
        m_isRunning = true;
        QFile file(m_filePath);
        if (file.open(QFile::ReadOnly))
        {
            AudioPlayerNS::AudioInfo audioInfo;
            audioInfo.fs = 44100;
            audioInfo.format = AudioPlayerNS::FORMAT_INT16;
            audioInfo.channels = 2;
            AudioPlayerNS::AudioPlayer ap;
            ap.init(audioInfo);
            ap.play();
            //逐段读取文件, 送入音频播放对象中
            int32 dataLen = 64 * 1024;
            char* pData = new char[dataLen];
            while (file.atEnd() && m_isRunning == true)
            {
                int realReadLen = file.read(pData, dataLen);
                if (realReadLen > 0)
                {
                    bool isPushed = false;
                    while (isPushed == false)
                    {
                        ap.pushData(pData, realReadLen);
                        msleep(500);
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
        m_isRunning = false;
        return;
    }

private:
    bool m_isRunning;
    QString m_filePath;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_Play_clicked();

private:
    Ui::Widget *ui;
    AudioPlayerNS::AudioPlayer m_ap;
};

#endif // WIDGET_H
