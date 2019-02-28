#include "widget.h"
#include "ui_widget.h"

extern "C" int testFunc(int a, int b);

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_Play_clicked()
{
#if 0
    m_thread.stop();
    m_thread.wait();

    AudioPlayerThreadParam param;
    param.filePath = "C:/test/Weight of the World.wav";
    m_thread.setParam(param);
    m_thread.start();
#endif
    AudioPlayerNS::AudioFileInfo fileInfo;
    fileInfo.path = "C:/test/Weight of the World.wav";
    fileInfo.fs = 22050;
    fileInfo.channeType = AudioPlayerNS::I_Q;
    fileInfo.dataFormat = AudioPlayerNS::FORMAT_INT16;
    fileInfo.startReadPos = 0;
    fileInfo.stopReadPos = -1;
    m_apThread.setPlayFile(fileInfo);
    m_apThread.play();
}
