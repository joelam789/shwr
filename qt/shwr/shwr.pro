#-------------------------------------------------
#
# Project created by QtCreator 2016-07-11T08:55:55
#
#-------------------------------------------------

QT       -= core gui

TARGET = shwr
TEMPLATE = lib

CONFIG += staticlib

DEFINES += SHWR_LIBRARY

SOURCES += shwr.cpp

HEADERS += shwr.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
