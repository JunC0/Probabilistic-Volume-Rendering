///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QUrl>
#include "window.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    QString fileName;
    QUrl filepath;

private slots:
    void open();
    void close();

private:
    void createActions();
    void createMenus();
    void loadFile(const QUrl &fileName);


    Window *c_window;
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;
    QMenu *selection;
    QAction *open_selection;
    QAction *save_selection;


};

#endif
