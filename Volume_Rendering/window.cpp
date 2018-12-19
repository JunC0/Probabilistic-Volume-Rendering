///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "glwidget.h"
#include "glwidget2.h"
#include "glwidget_select.h"
#include "window.h"
#include "mainwindow.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QTabWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QDockWidget>
#include <QTextEdit>
#include <QTime>


#define ALL_MAP_75D


Window::Window(MainWindow *mw)
    : mainWindow(mw)
{


    int ww=0;
    int hh=0;
    int dd=0;


    cc=0;

    int tf_num=11;

    for(int i=0;i<tf_num;i++){
        char t[100];
        sprintf(t,"histogram%d.txt",i);
        FILE *histogram=fopen(t,"w");
        for(int j=0;j<256;j++){
            fprintf(histogram,"0\n");
        }
        fclose(histogram);
    }

    for(int i=0;i<tf_num;i++){
        char t[30];
        sprintf(t,"color_table%d.txt",i);
        FILE *color_table=fopen(t,"w");
        for(int j=0;j<256;j++){
            fprintf(color_table,"0.0 0.0 0.0 0.0\n");
        }
        fclose(color_table);
    }




    glWidget_75d_all_map_rf = new GLWidget(mw->filepath,this);
    ww=glWidget_75d_all_map_rf->w;
    hh=glWidget_75d_all_map_rf->h;
    dd=glWidget_75d_all_map_rf->d;


    QHBoxLayout *container = new QHBoxLayout;
    QVBoxLayout *right_part=new QVBoxLayout;

    QHBoxLayout *selection_part=new QHBoxLayout;


    x_slice=new GLWidget_select(mw->filepath,1,0,0,this);
    x_slider=createSlider(ww-1);
    connect(x_slider,SIGNAL(valueChanged(int)),x_slice,SLOT(set_slice(int)));
    QVBoxLayout *x_select=new QVBoxLayout;
    x_select->addWidget(x_slice);
    x_select->addWidget(x_slider);


    y_slice=new GLWidget_select(mw->filepath,0,1,0,this);
    y_slider=createSlider(hh-1);
    connect(y_slider,SIGNAL(valueChanged(int)),y_slice,SLOT(set_slice(int)));
    QVBoxLayout *y_select=new QVBoxLayout;
    y_select->addWidget(y_slice);
    y_select->addWidget(y_slider);


    z_slice=new GLWidget_select(mw->filepath,0,0,1,this);
    z_slider=createSlider(dd-1);

   connect(z_slider,SIGNAL(valueChanged(int)),z_slice,SLOT(set_slice(int)));
    QVBoxLayout *z_select=new QVBoxLayout;
    z_select->addWidget(z_slice);
    z_select->addWidget(z_slider);

    brush_slider=createSlider(30);
    brush_slider->setRange(1,30);
    brush_slider->setPageStep(1);
    brush_slider->setTickInterval(1);
    connect(brush_slider,SIGNAL(valueChanged(int)),x_slice,SLOT(set_brush_size(int)));
    connect(brush_slider,SIGNAL(valueChanged(int)),y_slice,SLOT(set_brush_size(int)));
    connect(brush_slider,SIGNAL(valueChanged(int)),z_slice,SLOT(set_brush_size(int)));


    selection_part->addLayout(x_select);
    selection_part->addLayout(y_select);
    selection_part->addLayout(z_select);


    x_slice->setMouseTracking(true);
    y_slice->setMouseTracking(true);
    z_slice->setMouseTracking(true);


    cut_plane=createSlider(dd);
    cut_plane->setValue(dd);


    connect(cut_plane,SIGNAL(valueChanged(int)),this,SLOT(cut_flag2(int)));

    QLabel *apply_cut=new QLabel;
    apply_cut->setText("Cut_plane (z-axis)");


    connect(this,SIGNAL(cut_signal(int)),glWidget_75d_all_map_rf,SLOT(cut_plane(int)));



    QPushButton *apply=new QPushButton;
    apply->setText("Apply");

    connect(x_slice,SIGNAL(sigapply_75d_all_map_rf(int)),glWidget_75d_all_map_rf,SLOT(apply_75d_all_map_rf(int)));

    connect(apply,SIGNAL(clicked(bool)),x_slice,SLOT(apply(bool)));


    QPushButton *reset=new QPushButton;
    reset->setText("Reset");
    connect(reset,SIGNAL(clicked(bool)),x_slice,SLOT(reset(bool)));


    QTabWidget *tf_group=new QTabWidget;

    for(int i=0;i<tf_num;i++){
        tf[i]=new GLWidget2(i,this);
        char ttt[10];
        sprintf(ttt,"label: %d",i);
        tf_group->addTab(tf[i],QString(ttt));
    }
    connect(tf_group,SIGNAL(tabBarClicked(int)),x_slice,SLOT(label_change(int)));
    connect(tf_group,SIGNAL(tabBarClicked(int)),y_slice,SLOT(label_change(int)));
    connect(tf_group,SIGNAL(tabBarClicked(int)),z_slice,SLOT(label_change(int)));

    for(int i=0;i<tf_num;i++){
        connect(tf[i],SIGNAL(color_change(float,float,float)),x_slice,SLOT(color_change(float,float,float)));
        connect(tf[i],SIGNAL(color_change(float,float,float)),y_slice,SLOT(color_change(float,float,float)));
        connect(tf[i],SIGNAL(color_change(float,float,float)),z_slice,SLOT(color_change(float,float,float)));
    }

    //////////////////////////
        scale_z=new QSlider(Qt::Vertical);
        scale_z->setRange(1,500);
        scale_z->setSingleStep(1);
        scale_z->setPageStep(1);
        scale_z->setTickInterval(1);
        scale_z->setTickPosition(QSlider::TicksLeft);


        connect(scale_z,SIGNAL(valueChanged(int)),glWidget_75d_all_map_rf,SLOT(set_z_scale(int)));

        connect(x_slice,SIGNAL(sigset_histogram()),glWidget_75d_all_map_rf,SLOT(set_histogram()));

        scale_histogram=new QSlider(Qt::Vertical);
        scale_histogram->setRange(1,100);
        scale_histogram->setSingleStep(1);
        scale_histogram->setPageStep(1);
        scale_histogram->setTickInterval(1);
        scale_histogram->setTickPosition(QSlider::TicksLeft);

        for(int i=0;i<tf_num;i++){
            connect(scale_histogram,SIGNAL(valueChanged(int)),tf[i],SLOT(set_histogram_scale(int)));
        }

    //////////////////////////////////////////////////////////////////////////////////////////

    QHBoxLayout *tf_layout=new QHBoxLayout;
    tf_layout->addWidget(tf_group);
    tf_layout->addWidget(scale_histogram);
    //tf_layout->addWidget(scale_z);


    QHBoxLayout *cut_layout=new QHBoxLayout;
    cut_layout->addWidget(cut_plane);
    cut_layout->addWidget(apply_cut);


    for_magic_wand=new QSpinBox;
    for_magic_wand->setRange(-1,255);
    for_magic_wand->setValue(-1);
    connect(for_magic_wand,SIGNAL(valueChanged(int)),x_slice,SLOT(set_tolerance(int)));
    connect(for_magic_wand,SIGNAL(valueChanged(int)),y_slice,SLOT(set_tolerance(int)));
    connect(for_magic_wand,SIGNAL(valueChanged(int)),z_slice,SLOT(set_tolerance(int)));

//    for_magic_wand->setValue(1);

    QHBoxLayout *brush_setting=new QHBoxLayout;
    brush_setting->addWidget(brush_slider);
    brush_setting->addWidget(for_magic_wand);


    right_part->addLayout(tf_layout);
    right_part->addLayout(cut_layout);
    right_part->addWidget(apply);
    right_part->addWidget(reset);
    right_part->addLayout(brush_setting);
    right_part->addLayout(selection_part);


    QTabWidget *gl_75d_all_map_rf=new QTabWidget;
    gl_75d_all_map_rf->addTab(glWidget_75d_all_map_rf,"Our method");

    container->addWidget(gl_75d_all_map_rf);


    container->addLayout(right_part);
//

    connect(this,SIGNAL(o_selection2()),x_slice,SLOT(o_selection()));
    connect(this,SIGNAL(s_selection2()),x_slice,SLOT(s_selection()));





    ////light widget
    ///
    ///

    QSlider *ambi = new QSlider(Qt::Vertical);
    ambi->setRange(0, 300);
    ambi->setSingleStep(1);
    ambi->setTickInterval(10);
    ambi->setTickPosition(QSlider::TicksRight);
    ambi->setValue(35);

    QLabel *ambi_b=new QLabel;
    ambi_b->setText("Amb");


    QVBoxLayout *am=new QVBoxLayout;
    am->addWidget(ambi);
    am->addWidget(ambi_b);

    QSlider *diff = new QSlider(Qt::Vertical);
    diff->setRange(0, 100);
    diff->setSingleStep(1);
    diff->setTickInterval(10);
    diff->setTickPosition(QSlider::TicksRight);
    diff->setValue(10);

    QLabel *diff_b=new QLabel;
    diff_b->setText("Dif");

    QVBoxLayout *di=new QVBoxLayout;
    di->addWidget(diff);
    di->addWidget(diff_b);


    QSlider *spec = new QSlider(Qt::Vertical);
    spec->setRange(0, 100);
    spec->setSingleStep(1);
    spec->setTickInterval(10);
    spec->setTickPosition(QSlider::TicksRight);
    spec->setValue(10);

    QLabel *spec_b=new QLabel;
    spec_b->setText("Spec");

    QVBoxLayout *sp=new QVBoxLayout;
    sp->addWidget(spec);
    sp->addWidget(spec_b);

    QSlider *shin = new QSlider(Qt::Vertical);
    shin->setRange(0, 100);
    shin->setSingleStep(1);
    shin->setTickInterval(10);
    shin->setTickPosition(QSlider::TicksRight);
    shin->setValue(8);

    QLabel *shin_b=new QLabel;
    shin_b->setText("Shin");

    QVBoxLayout *sh=new QVBoxLayout;
    sh->addWidget(shin);
    sh->addWidget(shin_b);

    QSlider *l_t = new QSlider(Qt::Vertical);
    l_t->setRange(0, 100);
    l_t->setSingleStep(1);
    l_t->setTickInterval(10);
    l_t->setTickPosition(QSlider::TicksRight);
    l_t->setValue(8);

    QLabel *l_t_b=new QLabel;
    l_t_b->setText("LL");

    QVBoxLayout *l_=new QVBoxLayout;
    l_->addWidget(l_t);
    l_->addWidget(l_t_b);


    connect(ambi,SIGNAL(valueChanged(int)),glWidget_75d_all_map_rf,SLOT(ambi_change(int)));
    connect(diff,SIGNAL(valueChanged(int)),glWidget_75d_all_map_rf,SLOT(diff_change(int)));
    connect(spec,SIGNAL(valueChanged(int)),glWidget_75d_all_map_rf,SLOT(spec_change(int)));
    connect(shin,SIGNAL(valueChanged(int)),glWidget_75d_all_map_rf,SLOT(shin_change(int)));
    connect(l_t,SIGNAL(valueChanged(int)),glWidget_75d_all_map_rf,SLOT(l_t_change(int)));


    QHBoxLayout *whole_frame=new QHBoxLayout;
    whole_frame->addLayout(container);
    whole_frame->addLayout(am);
    whole_frame->addLayout(di);
    whole_frame->addLayout(sp);
    whole_frame->addLayout(sh);
    whole_frame->addLayout(l_);


    setLayout(whole_frame);
    setWindowTitle(tr("Volume Rendering"));


    QWidget::setFocusPolicy(Qt::ClickFocus);
}


void Window::cut_flag2(int a){
    emit cut_signal(a);
}


QSlider *Window::createSlider(int a)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, a);
    slider->setSingleStep(1);
    slider->setPageStep(16);
    slider->setTickInterval(8);
    slider->setTickPosition(QSlider::TicksAbove);
    return slider;
}
void Window::s_selection(){
    emit s_selection2();
}
void Window::o_selection(){
    emit o_selection2();
}
