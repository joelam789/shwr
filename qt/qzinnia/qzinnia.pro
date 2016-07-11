#-------------------------------------------------
#
# Project created by QtCreator 2016-07-08T17:20:26
#
#-------------------------------------------------

QT       -= gui

TARGET = qzinnia
TEMPLATE = lib

CONFIG += staticlib

#CONFIG += release warn_off

DEFINES += QZINNIA_LIBRARY

DEFINES += DLL_EXPORT HAVE_CONFIG_H

#For Windows
DEFINES += HAVE_WINDOWS_H

SOURCES += qzinnia.cpp \
    character.cpp \
    feature.cpp \
    libzinnia.cpp \
    param.cpp \
    recognizer.cpp \
    sexp.cpp \
    svm.cpp \
    trainer.cpp \
    zinnia.cpp \
    zinnia_convert.cpp \
    zinnia_learn.cpp

HEADERS += qzinnia.h\
    common.h \
    config.h \
    feature.h \
    freelist.h \
    mmap.h \
    param.h \
    scoped_ptr.h \
    sexp.h \
    stream_wrapper.h \
    svm.h \
    zinnia.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
