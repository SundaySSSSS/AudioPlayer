#-------------------------------------------------
#
# Project created by QtCreator 2019-01-23T10:57:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioPlayer
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    AudioPlayer.cpp

HEADERS  += widget.h \
    AudioPlayer.h

FORMS    += widget.ui

LIBS += -L../bin -lSDL2

DESTDIR = ../bin
