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
    AudioPlayerNS::AudioFileInfo fileInfo;
    fileInfo.path = "C:/test/Weight of the World.wav";
    fileInfo.fs = 22050;
    fileInfo.channeType = AudioPlayerNS::I_Q;
    fileInfo.dataFormat = AudioPlayerNS::FORMAT_INT16;
    fileInfo.startReadPos = 0;
    fileInfo.stopReadPos = -1;
    m_apThread.init(fileInfo);
    m_apThread.play();
}

void Widget::on_pushButton_Pause_clicked()
{
    m_apThread.pause();
}
