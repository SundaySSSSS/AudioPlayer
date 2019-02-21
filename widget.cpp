#include "widget.h"
#include "ui_widget.h"

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
    AudioPlayerNS::AudioInfo audioInfo;
    audioInfo.fs = 44100;
    audioInfo.format = AudioPlayerNS::FORMAT_INT16;
    audioInfo.channels = 2;

    //m_ap.init(audioInfo);
    m_ap.playWav("C:/test/Weight of the World.wav");
    //m_ap.play();
}
