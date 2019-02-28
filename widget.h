#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFile>
#include <QThread>
#include <QDebug>
#include "AudioPlayer.h"
#include "AudioPlayerThread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_Play_clicked();

    void on_pushButton_Pause_clicked();

    void on_pushButton_Stop_clicked();

    void on_horizontalSlider_Volume_valueChanged(int value);

private:
    Ui::Widget *ui;
    AudioPlayerNS::AudioPlayer m_ap;
    ReadFileThread m_thread;
    AudioPlayerNS::AudioPlayerThread m_apThread;
};

#endif // WIDGET_H
