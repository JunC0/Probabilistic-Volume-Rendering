##JunYoung Choi
##juny0603@gmail.com
##Ulsan National Institute of Science and Technology(UNIST)

HEADERS       = glwidget.h \
                glwidget2.h \
                glwidget_select.h \
                window.h \
                mainwindow.h
SOURCES       = glwidget.cpp \
                glwidget2.cpp \
                glwidget_select.cpp \
                main.cpp \
                window.cpp \
                mainwindow.cpp


QT           +=widgets
QT           +=gui


TARGET = Volume_Rendering
TEMPLATE = app
