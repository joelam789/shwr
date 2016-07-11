#-------------------------------------------------
#
# Project created by QtCreator 2016-07-11T09:00:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shwrtest
TEMPLATE = app

LIBS += ./lib/libqzinnia.a
LIBS += ./lib/libshwr.a

SOURCES += main.cpp\
        mainwindow.cpp \
    handwritingcanvas.cpp

HEADERS  += mainwindow.h \
    qzinnia.h \
    shwr.h \
    handwritingcanvas.h

FORMS    += mainwindow.ui
