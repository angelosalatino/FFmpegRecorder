#-------------------------------------------------
#
# Project created by QtCreator 2014-02-10T19:25:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FFmpegRecorder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    options.cpp

HEADERS  += mainwindow.h \
    options.h

FORMS    += mainwindow.ui \
    options.ui
