///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include <QProcess>
#include <QApplication>
#include <QDesktopWidget>
#include <QSurfaceFormat>
#include "mainwindow.h"
#include <windows.h>
int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    MainWindow *mainWindow;
    mainWindow = new MainWindow;
    mainWindow->setBaseSize(1600,800);     //resolution
    mainWindow->setMinimumSize(1600,800); //resolution
    mainWindow->show();

    return app.exec();
}
