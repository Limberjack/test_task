TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += \
        devise.cpp \
        main.cpp \
        serialport.cpp

HEADERS += \
    devise.h \
    serialport.h
