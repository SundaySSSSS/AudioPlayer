#include "widget.h"
#include <QApplication>

#include "AudioPlayer.h"

#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.show();

    return a.exec();
}
