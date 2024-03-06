QT          += core gui widgets printsupport

CONFIG      += c++17
CONFIG      += lrelease

#TARGET      = SmartCalc_v2.0

TEMPLATE    = app

INCLUDEPATH = include

OBJECTS_DIR = obj

MOC_DIR     = moc

DESTDIR     = ../

TARGETDEPS  += ../Makefile

SOURCES     += \
    source/main.cc \
    source/model_calc.cc \
    source/model_finance.cc \
    source/view.cc \
    source/qcustomplot.cc \
    source/controller.cc \

HEADERS     += \
    include/view.h \
    include/model.h \
    include/model_calc.h \
    include/model_finance.h \
    include/qcustomplot.h \
    include/controller.h \
    include/utils.h

FORMS       += \
    form/mainwindow.ui
