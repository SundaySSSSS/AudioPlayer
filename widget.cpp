#include "widget.h"
#include "ui_widget.h"

extern "C" int testFunc(int a, int b);

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    qDebug() << testFunc(1, 2);
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_Play_clicked()
{
    AudioPlayerNS::AudioInfo audioInfo;
    audioInfo.fs = 44100;
    audioInfo.format = AudioPlayerNS::FORMAT_INT16;
    audioInfo.channels = 2;
    audioInfo.filePath = "C:/test/Weight of the World.wav";

    //m_ap.init(audioInfo);
    m_ap.playWav("C:/test/Weight of the World.wav");
    //m_ap.play();
}
