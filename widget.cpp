#include "widget.h"
#include "ui_widget.h"

#include <QMessageBox>

#define PLAY_RATE_MAX 100

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->pushButton_Play->setEnabled(true);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Stop->setEnabled(false);

    ui->horizontalSlider_Volume->setRange(0, 128);
    ui->horizontalSlider_Volume->setValue(64);

    ui->horizontalSlider_PlayRate->setRange(0, PLAY_RATE_MAX);
    ui->horizontalSlider_PlayRate->setValue(0);

    ui->lineEdit_Path->setText("C:/test/Weight of the World.wav");

    connect(&m_apThread, SIGNAL(sendPlayFileProcess(float)), this, SLOT(recvPlayedProcess(float)));
    connect(&m_apThread, SIGNAL(sendPlayFinished()), this, SLOT(recvPlayFinished()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_Play_clicked()
{
    AudioPlayerNS::AudioFileInfo fileInfo;
    fileInfo.path = ui->lineEdit_Path->text();
    fileInfo.fs = 22050;
    fileInfo.channeType = AudioPlayerNS::I_Q;
    fileInfo.dataFormat = AudioPlayerNS::FORMAT_INT16;
    fileInfo.startReadPos = 16000000;
    fileInfo.stopReadPos = -1;
    m_apThread.init(fileInfo);
    m_apThread.play();
    ui->pushButton_Play->setEnabled(false);
    ui->pushButton_Pause->setEnabled(true);
    ui->pushButton_Stop->setEnabled(true);
}

void Widget::on_pushButton_Pause_clicked()
{
    if (ui->pushButton_Pause->text() == "暂停")
    {
        ui->pushButton_Pause->setText("恢复");
        m_apThread.pause();
    }
    else
    {
        ui->pushButton_Pause->setText("暂停");
        m_apThread.resume();
    }
}

void Widget::on_pushButton_Stop_clicked()
{
    m_apThread.stop();
    ui->pushButton_Play->setEnabled(true);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Stop->setEnabled(false);
}

void Widget::on_horizontalSlider_Volume_valueChanged(int value)
{
    m_apThread.setVolume(value);
}

void Widget::recvPlayedProcess(float rate)
{
    int32 value = rate * PLAY_RATE_MAX;
    ui->horizontalSlider_PlayRate->setValue(value);
}

void Widget::recvPlayFinished()
{
    ui->horizontalSlider_PlayRate->setValue(PLAY_RATE_MAX);
    m_apThread.stop();
    ui->pushButton_Play->setEnabled(true);
    ui->pushButton_Pause->setEnabled(false);
    ui->pushButton_Stop->setEnabled(false);
}

void Widget::on_pushButton_MemPlay_clicked()
{
    AudioPlayerNS::AudioMemInfo memInfo;
    memInfo.fs = 22050;
    memInfo.channeType = AudioPlayerNS::I_Q;
    memInfo.dataFormat = AudioPlayerNS::FORMAT_INT16;
    if (!m_apThread.init(memInfo))
    {
        QMessageBox::warning(this, "错误", "内存模式初始化错误");
        return;
    }

    //从文件中读取数据
    QString path = ui->lineEdit_Path->text();
    QFile file(path);
    if (file.open(QFile::ReadOnly))
    {
        file.seek(20 * 1024 * 1024);
        int32 len = 16 * 1024 * 2 * 2;
        char* pBuf = new char[16 * 1024 * 2 * 2];
        int32 readLen = file.read(pBuf, len);
        m_apThread.push(pBuf, readLen);
        file.close();
        delete[] pBuf;
        m_apThread.play();
    }
    else
    {
        QMessageBox::warning(this, "错误", "文件打开失败");
    }
}
