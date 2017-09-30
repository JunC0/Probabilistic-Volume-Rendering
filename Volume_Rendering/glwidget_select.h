///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#ifndef GLWIDGET_SELECT_H
#define GLWIDGET_SELECT_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>

class Window;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget_select : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Compatibility
{
    Q_OBJECT

public:
    GLWidget_select(QUrl fileinfo, int xx, int yy, int zz, Window *parent,QWidget *parent1 = 0);
    ~GLWidget_select();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void magicWand(int z, int y, int x, int intensity);

    std::string get_file_location(std::string inform);

public slots:

    void cleanup();
    void set_slice(int a){
        slx=axx*a;
        sly=axy*a;
        slz=axz*a;
    }
    void set_brush_size(int a);
    void apply(bool a);
    void reset(bool a);
    void label_change(int n);
    void color_change(float r,float g,float b);

    void o_selection();
    void s_selection();
    void set_tolerance(int to);

signals:
    void sigapply_8d(int max_label);
    void sigapply_11d(int max_label);
    void sigapply_72d_all_map(int max_label);
    void sigapply_75d_all_map(int max_label);

    void sigapply_8d_rf(int max_label);
    void sigapply_11d_rf(int max_label);
    void sigapply_72d_all_map_rf(int max_label);
    void sigapply_75d_all_map_rf(int max_label);

    void sigapply_8d_svm(int max_label);
    void sigapply_11d_svm(int max_label);
    void sigapply_72d_all_map_svm(int max_label);
    void sigapply_75d_all_map_svm(int max_label);

    void sigapply_27d_f_map_rf(int max_label);
    void sigapply_51d_fr_map_rf(int max_label);

    void sigapply_75d_all_map_xgb(int max_label);

    void sigapply_75(int max_label);
    void sigapply_h_map_rf(int max_label);
    void sigset_histogram();

    void sigapply_auto(int max_label);


    void sigapply_75d_all_map_haejin2(int max_label);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    QVector3D cross_product(QVector3D v1,QVector3D v2);
    void ReadColorTable();

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private:
    QPoint m_lastPos;

    int slice;
    float size_w,size_h;

    QVector4D color_table[256];

    float *n_data;

    float window_rate_w,window_rate_h;

    float scale;
    int w,h,d;
    int dx,dy;
    std::string filename;

    int select_tog;

    int axx,axy,axz;

    int slx,sly,slz;


    unsigned char *data;

    int cur_select_label;
    unsigned char t_buffer[50];

    int zoom;
    int select_zoom_range;

    QPoint zoom_start,zoom_end;

    QPoint view_start,view_end;

    int mw;
    int tolerance;
    int cc;

    Window *c_window;
    int t_size;

};

#endif
