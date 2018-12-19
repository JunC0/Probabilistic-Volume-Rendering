///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMatrix4x4>
#include <QLabel>
QT_BEGIN_NAMESPACE
class QSlider;
class QPushButton;
QT_END_NAMESPACE

class GLWidget;
class MainWindow;
class GLWidget_select;
class GLWidget2;
class QSpinBox;
class QTimer;

class QTextEdit;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(MainWindow *mw);

    int check[305][305][256];

    QVector3D label_color[27];

protected:

signals:

    void cut_signal(int a);
    void o_selection2();
    void s_selection2();


    void save_result(QString b);

private slots:


    void cut_flag2(int a);
    void o_selection();
    void s_selection();



private:

    QSlider *createSlider(int a);

    GLWidget *glWidget_75d_all_map_rf;

    GLWidget_select *glWidget2;
    MainWindow *mainWindow;


    QSlider *Slider;

    QSlider *cut_plane;


    int atom_num;

    GLWidget_select *x_slice;
    GLWidget_select *y_slice;
    GLWidget_select *z_slice;
    QSlider *x_slider;
    QSlider *y_slider;
    QSlider *z_slider;

    QSlider *brush_slider;
    QSlider *see_cut_slider;


    QSlider *scale_histogram;
    QSlider *scale_z;
    GLWidget2 * tf[13];
    QSpinBox *for_magic_wand;

    int cc;
};

#endif
