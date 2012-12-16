QT       += core gui network

TARGET = pop3Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pop.cpp

HEADERS  += mainwindow.h \
    pop.h \
    structs.h

FORMS    += mainwindow.ui
