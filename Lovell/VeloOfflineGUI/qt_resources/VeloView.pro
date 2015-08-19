#-------------------------------------------------
#
# Project created by QtCreator 2014-01-31T09:35:40
#
#-------------------------------------------------


#cache()
QT       += core
CONFIG += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
TEMPLATE = app

TARGET = VView
OBJECTS_DIR = tmp

SOURCES += main.cpp\
           src/VeloView.cpp\
           src/VContentGetter.cpp\
           qcustomplot.cpp \
           src/VPlot.cpp\
           src/treemodel.cpp \
           src/treeitem.cpp \
           src/VCustomPlot.cpp \
           src/VPlottable.cpp \
           src/VTabContent.cpp \
           src/VTable.cpp \
           src/VTree.cpp \
           src/VCombatContent.cpp \
           src/VKeplerContent.cpp \
           src/VPlotOps.cpp 

HEADERS  += headers/VeloView.h\
            headers/VPlot.h\
            headers/treemodel.h \
            headers/treeitem.h \
            headers/VPlottable.h\
            headers/VTabContent.h\
            headers/VContentGetter.h\
            qcustomplot.h \
            headers/VCustomPlot.h \
            headers/VTable.h \
            headers/VTree.h \
            headers/VCombatContent.h \
            headers/VKeplerContent.h \
            headers/VPlotOps.h \

FORMS    += VeloView.ui
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += `root-config --cflags --libs`
QMAKE_LIBS += `root-config --libs`
QMAKE_INCPATH += `root-config --incdir`
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
