TEMPLATE = app
CONFIG += console

CONFIG -= app_bundle
CONFIG -= qt

CONFIG += link_pkgconfig

PKGCONFIG += opencv

CXXFLAGS += \
    -Wwrite-strings


#INCLUDEPATH +=

SOURCES += \
    ../src/cvSetCard.cpp \
    ../src/cvSetGameBoard.cpp \
    ../src/main.cpp \
    ../src/cvSetUtils.cpp

HEADERS += \
    ../src/cvSetCard.h \
    ../src/cvSetGameBoard.h \
    ../src/cvSetUtils.h
