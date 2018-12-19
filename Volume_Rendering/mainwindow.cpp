///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include <QtWidgets>

#include "mainwindow.h"
#include "window.h"
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QDockWidget>

#include "user_control2.h"
MainWindow::MainWindow()
{

    setAttribute(Qt::WA_DeleteOnClose);
    createActions();
    createMenus();
    (void)statusBar();
    setWindowFilePath(QString());
}


void MainWindow::open()
{
    filepath = QFileDialog::getOpenFileUrl(this);
    if (!filepath.fileName().isEmpty()){
        loadFile(filepath);
        c_window=new Window(this);
        connect(open_selection, SIGNAL(triggered()), c_window, SLOT(o_selection()));
        connect(save_selection, SIGNAL(triggered()), c_window, SLOT(s_selection()));
        setCentralWidget(c_window);
    }
}


void MainWindow::close()
{
    setCentralWidget(NULL);
    delete [] c_window;
    setWindowFilePath(QString());
}

void MainWindow::createActions()
{

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));


    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    open_selection = new QAction(tr("Import"), this);
    save_selection = new QAction(tr("Export"), this);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);


    selection = menuBar()->addMenu(tr("&Selection"));
    selection->addAction(open_selection);
    selection->addSeparator();
    selection->addAction(save_selection);

}

void MainWindow::loadFile(const QUrl &fileName)
{
    setWindowFilePath(fileName.toString());
}

