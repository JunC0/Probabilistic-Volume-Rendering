///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Compatibility>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QVector2D>


class Window;

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Compatibility
{
    Q_OBJECT

public:
    GLWidget(QUrl fileinfo,Window *p,QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    int w,h,d;

public slots:

    void cleanup();
    void apply_75d_all_map_rf(int max_label);

    void cut_plane(int r);

    void set_histogram();

    void set_z_scale(int n);

    void save_result(QString a);

    void ambi_change(int a);
    void diff_change(int a);
    void spec_change(int a);
    void shin_change(int a);
    void l_t_change(int a);



signals:


protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
    void Load3DFile();
    QVector3D cross_product(QVector3D v1,QVector3D v2);
    void ReadColorTable();

    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    void set_probability(int max_label);


private:
    QPoint m_lastPos;

    QOpenGLShaderProgram *m_program;

    GLuint tex_3D;
    GLuint gradient;
    GLuint gradient2;

    QVector3D box_min;
    QVector3D box_max;


    int con_eye_pos;
    int con_box_min;
    int con_box_max;
    int con_up;
    int con_center;



    int con_tex;
    int con_gradient;
    int con_gradient2;

    QVector3D const_box_max;
    QVector3D const_box_min;
    QVector4D color_table[11][256];


    int con_color_table[11];

    GLuint color_tex[11];



    int con_sample;
    float sample;



    int con_l_t;

    std::string filename;

    unsigned char *data;

    int reset_color;

    float max;
    int con_max;
    int *input;
    unsigned char *data2;

    float z_scale;

    QVector4D background_color;
    int con_background_color;

    int tf_num;



    Window *c_window;



    int con_specu,con_diffu,con_ambi,con_shin;



    unsigned char * probability[10];

    GLuint proba_tex[10];
    int con_proba_tex[10];

    QVector2D shift;

    int fps_start;
    int fps;
    QString p_fps;
};

#endif
