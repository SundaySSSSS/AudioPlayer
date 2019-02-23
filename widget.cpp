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
    m_thread.stop();
    m_thread.wait();

    AudioPlayerThreadParam param;
    param.filePath = "C:/test/Weight of the World.wav";
    m_thread.setParam(param);
    m_thread.start();
}
