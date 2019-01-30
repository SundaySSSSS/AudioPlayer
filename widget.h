#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "AudioPlayer.h"

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

private:
    Ui::Widget *ui;
    AudioPlayerNS::AudioPlayer m_ap;
};

#endif // WIDGET_H
