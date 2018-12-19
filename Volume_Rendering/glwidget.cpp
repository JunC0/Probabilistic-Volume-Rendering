///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////


#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QProcess>
#include <QColorDialog>
#include <window.h>
#include <QtWidgets>

QVector3D eye_position;
QVector3D center1;
QVector3D up;
int tog;
int l_t;
float scale;


float ambi=0.7,diffu=0.1,specu=0.1,shin=8;


int n_map2=24;
int h_map2=72;
int is_loc=3;
int add_o2=8;
float EYE=5;

void GLWidget::Load3DFile(){

    sample=sqrt(float(w*w+h*h+d*d))/2;

    FILE *f2=fopen(filename.c_str(),"rb");
    data=new unsigned char[w*h*d];
    data2=new unsigned char[w*h*d];
    fread(data,w*h*d,1,f2);
    fclose(f2);
    float *G=new float[w*h*d*4];
    float *G2=new float[w*h*d*4];

    for(int i=0;i<w*h*d;i++){
        G[i*4+0]=0,G[i*4+1]=0,G[i*4+2]=0,G[i*4+3]=float(data[i])/255.0,
            data2[i]=0;
        G2[i*4+0]=0,G2[i*4+1]=0,G2[i*4+2]=0,G2[i*4+3]=0;
    }


    max=0;
    for(int z=1;z<d-1;z++){
        for(int y=1;y<h-1;y++){
            for(int x=1;x<w-1;x++){
                int cur=z*w*h+y*w+x;
                int xm1=cur-1;
                int xp1=cur+1;
                int ym1=cur-w;
                int yp1=cur+w;
                int zm1=cur-w*h;
                int zp1=cur+w*h;
                float t1=(float(data[xp1])-float(data[xm1]));
                float t2=(float(data[yp1])-float(data[ym1]));
                float t3=(float(data[zp1])-float(data[zm1]));
                float gt=sqrt(t1*t1+t2*t2+t3*t3);
                if(gt>max)max=gt;
                G[cur*4+0]=abs(t1/255.0);
                G[cur*4+1]=abs(t2/255.0);
                G[cur*4+2]=abs(t3/255.0);
                if(t1<0)G2[cur*4+0]=1;
                if(t2<0)G2[cur*4+1]=1;
                if(t3<0)G2[cur*4+2]=1;


            }
        }
    }


    int histo[256];
    int total_histo;
    for(int i=0;i<256;i++){
        histo[i]=0;
    }
    total_histo=0;
    for(int i=0;i<w*h*d;i++){
        histo[data[i]]++;
        total_histo++;
    }

    FILE *histogram=fopen("histogram0.txt","w");
    for(int j=0;j<256;j++){
        fprintf(histogram,"%f\n",float(histo[j])/total_histo);
    }
    fclose(histogram);


    glGenTextures(1,&gradient);
    glBindTexture(GL_TEXTURE_3D,gradient);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,w,h,d,0,GL_RGBA,GL_FLOAT,G);

    glGenTextures(1,&gradient2);
    glBindTexture(GL_TEXTURE_3D,gradient2);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,w,h,d,0,GL_RGBA,GL_FLOAT,G2);

    glGenTextures(1,&tex_3D);
    glBindTexture(GL_TEXTURE_3D,tex_3D);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

    glTexImage3D(GL_TEXTURE_3D,0,GL_RED,w,h,d,0,GL_RED,GL_UNSIGNED_BYTE,data2);

    delete [] G,G2;
}

QVector3D GLWidget::cross_product(QVector3D v1,QVector3D v2){
    QVector3D v;
    v.setX(v1.y()*v2.z()-v2.y()*v1.z());
    v.setY(v1.z()*v2.x()-v2.z()*v1.x());
    v.setZ(v1.x()*v2.y()-v2.x()*v1.y());
    return v;
}

GLWidget::GLWidget(QUrl fileinfo, Window *p, QWidget *parent)
    : QOpenGLWidget(parent),
      m_program(0)
{
    c_window=p;
    tf_num=10;

    z_scale=1;

    reset_color=0;

    center1=QVector3D(0,0,0);
    eye_position=QVector3D(0,0,EYE);
    up=QVector3D(0,1,0);
    shift=QVector2D(0,0);

    tog=0;
    scale=1;
    startTimer(30);

    p_fps="";
    fps_start=-1;
    fps=0;

    QWidget::setFocusPolicy(Qt::ClickFocus);

    std::string ft=(fileinfo.toString(0)).toStdString();
    for(int i=8;i<ft.size();i++){
        if(ft[i]=='/'){
            filename+="\\\\";
        }
        else{
            filename+=ft[i];
        }
    }

    std::string t=(fileinfo.fileName()).toStdString();
    w=-1,h=-1,d=-1;
    for(int i=0;i<t.size();i++){
        if(t.c_str()[i]=='_'){
            int tt=0;
            i++;
            while(true){
                if(t.c_str()[i]=='_' || t.c_str()[i]=='.')break;
                tt*=10;
                tt+=t.c_str()[i]-'0';
                i++;
            }
            i--;
            if(w==-1)w=tt;
            else if(h==-1)h=tt;
            else if(d==-1)d=tt;
        }
    }
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(1600, 1600);
}

QSize GLWidget::sizeHint() const
{
    return QSize(1600, 1600);
}



void GLWidget::cleanup()
{
    makeCurrent();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

static const char *vertexShaderSource =
    "#version 140\n"
    "#extension GL_ARB_compatibility: enable\n\n"

    "out vec3 pixel_position;\n\n"

    "void main(){\n"
    "    pixel_position=vec3(gl_Vertex);\n"
    "    gl_Position =gl_Vertex;\n"
    "}\n";

static const char *fragmentShaderSource =
    "#version 140\n"
    "#extension GL_ARB_compatibility: enable\n"


    "uniform vec3 eye_position;\n"
    "uniform sampler3D tex;\n"
    "uniform sampler3D G;\n"
    "uniform sampler3D G2;\n"
    "in vec3 pixel_position;\n"
    "uniform vec3 box_min;\n"
    "uniform vec3 box_max;\n"
    "uniform vec3 up;\n"
    "uniform float Udiffu;\n"
    "uniform float Uambi;\n"
    "uniform float Uspecu;\n"
    "uniform float Ushin;\n"
    "uniform int l_t;\n"

    "uniform float sample;\n"

    "uniform sampler1D color_table[14];\n"
    "uniform vec4 background_color;\n"

    "uniform sampler3D proba_tex[10];\n"


   " vec3 cross_product(vec3 v1,vec3 v2){\n"
    "    vec3 v;\n"
    "    v.x=v1.y*v2.z-v2.y*v1.z;\n"
    "    v.y=v1.z*v2.x-v2.z*v1.x;\n"
    "    v.z=v1.x*v2.y-v2.x*v1.y;\n"
    "    return v;\n"
   " }\n"

    "void main()\n"
    "{	\n"
        "vec3 right=normalize(cross_product(-eye_position,up));\n"
     "   vec3 t_pixel_position=eye_position*0.5+"
                                "right*pixel_position.x+"
                                "up*pixel_position.y;\n"

//    "    vec3 direction=normalize(t_pixel_position-eye_position);\n"
     "    vec3 direction=normalize(-eye_position);\n"

"        int cn=0;\n"
    "    vec3 a[2];\n"
    "  float k;\n"
      "vec3 point;\n"
   "   if(cn<2){\n"
      "   k=(box_min.x-t_pixel_position.x)/direction.x;\n"
      "   point=t_pixel_position+k*direction;\n"
  "       if(point.y>=box_min.y && point.y<=box_max.y\n"
  "            && point.z>=box_min.z && point.z<=box_max.z){\n"
  "                a[cn++]=point;\n"
  "       }\n"
      "}\n"
  "    if(cn<2){\n"
      "   k=(box_max.x-t_pixel_position.x)/direction.x;\n"
      "   point=t_pixel_position+k*direction;\n"
  "       if(point.y>=box_min.y && point.y<=box_max.y\n"
  "            && point.z>=box_min.z && point.z<=box_max.z){\n"
  "                a[cn++]=point;\n"
  "       }\n"
  "    }\n"
  "    if(cn<2){\n"
      "   k=(box_min.y-t_pixel_position.y)/direction.y;\n"
      "   point=t_pixel_position+k*direction;\n"
  "       if(point.x>=box_min.x && point.x<=box_max.x\n"
  "            && point.z>=box_min.z && point.z<=box_max.z){\n"
  "                a[cn++]=point;\n"
  "       }\n"
  "    }\n"
  "    if(cn<2){\n"
  "      k=(box_max.y-t_pixel_position.y)/direction.y;\n"
  "       point=t_pixel_position+k*direction;\n"
  "       if(point.x>=box_min.x && point.x<=box_max.x\n"
  "            && point.z>=box_min.z && point.z<=box_max.z){\n"
  "                a[cn++]=point;\n"
  "       }\n"
  "    }\n"
  "    if(cn<2){\n"
  "      k=(box_min.z-t_pixel_position.z)/direction.z;\n"
  "       point=t_pixel_position+k*direction;\n"
  "       if(point.x>=box_min.x && point.x<=box_max.x\n"
  "            && point.y>=box_min.y && point.y<=box_max.y){\n"
  "                a[cn++]=point;\n"
  "       }\n"
  "    }\n"
  "    if(cn<2){\n"
  "      k=(box_max.z-t_pixel_position.z)/direction.z;\n"
  "       point=t_pixel_position+k*direction;\n"
  "       if(point.x>=box_min.x && point.x<=box_max.x\n"
  "            && point.y>=box_min.y && point.y<=box_max.y){\n"
  "                a[cn++]=point;\n"
  "       }\n"
  "    }\n"
    "   float alpha=0;\n"
    "    vec4 cur_color=vec4(0,0,0,0);	\n"
"        if(cn==2) {\n"

"            if(length(a[0]-eye_position)>length(a[1]-eye_position)){\n"
"                 vec3 t=a[0];\n"
"                 a[0]=a[1];\n"
"                 a[1]=t;\n   "
"            }\n"


"            int sampling_num=int(length(a[1]-a[0])*sample)*3;\n"
"            vec3 normalize_box=1.0/(box_max-box_min);\n"
"            vec3 dir=(a[1]-a[0])*normalize_box/sampling_num;\n"
"            vec3 cur_location=(a[0]-box_min)*normalize_box;\n"

"            int light_cnt=0;\n"
"            int first_flag=0;\n"
"            for(int i=0;i<sampling_num;i++){\n"
"                cur_location=cur_location+dir;\n"


"                vec4 apply=texture3D(G,cur_location);\n"
"                vec4 apply2=texture3D(G2,cur_location);\n"

"                if(apply2.x>0)apply.x=-apply.x;\n"
"                if(apply2.y>0)apply.y=-apply.y;\n"
"                if(apply2.z>0)apply.z=-apply.z;\n"

"                int apply_alpha=int(apply.w*255.0);\n"
"                vec3 gr=normalize(vec3(apply));\n"
"                vec3 d_l=eye_position*3+2*up+2*right;\n"
"                vec4 t_color=vec4(0,0,0,0);\n"
"                for(int i=0;i<8;i++){\n"
"                    float proba=texture3D(proba_tex[i],cur_location).x;\n"
"                    vec4 tt_color=texture1D(color_table[i],proba);\n"
"                    t_color=t_color+vec4(tt_color.x*tt_color.w,tt_color.y*tt_color.w,tt_color.z*tt_color.w,tt_color.w)*proba;\n"
"                }\n"
"                float t_alpha=t_color.w;\n"
"                if(t_color.w>0.004){\n"
"                    if(light_cnt<l_t){"
"                        first_flag=1;\n"
"                        light_cnt=light_cnt+1;\n"
"                        vec3 L=normalize(d_l-cur_location+vec3(0.5,0.5,0.5));\n"
"                        vec3 E=normalize(eye_position-cur_location+vec3(0.5,0.5,0.5));\n"
"                        vec3 R=normalize(-reflect(L,-gr));\n"
"                        vec4 ambi=vec4(Uambi,Uambi,Uambi,Uambi);\n"
"                        vec4 diffu=vec4(Udiffu,Udiffu,Udiffu,Udiffu)*max(dot(-gr,L),0.0);\n"
"                        diffu=clamp(diffu,0.0,1.0);\n"
"                        vec4 specu=vec4(Uspecu,Uspecu,Uspecu,Uspecu)*pow(max(dot(R,E),0.0),Ushin);\n"
"                        specu=clamp(specu,0.0,1.0);\n"
"                        t_color=t_color*ambi+diffu+specu;\n"
"                    }\n"
"                    else{\n"
"                        float ttx=length(vec3(apply));\n"
"                        t_color=t_color*ttx;\n"
"                    }\n"
"                    if(first_flag==1 || l_t==0){\n"
"                        cur_color=cur_color+t_color*(1-alpha);\n"
"                        alpha=alpha+(1-alpha)*t_alpha;\n"
"                        if(alpha>0.99)break;\n"
"                    }\n"
"                }\n"
"            }\n"
"        }\n"
"        cur_color=cur_color+background_color*(1-alpha);\n"
"        gl_FragColor=cur_color;\n"
"    }\n";






void GLWidget::initializeGL()
{

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    glClearColor(1, 1, 1,1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,fragmentShaderSource);

    m_program->link();

    m_program->bind();

    con_tex=m_program->uniformLocation("tex");



    con_gradient=m_program->uniformLocation("G");
    con_gradient2=m_program->uniformLocation("G2");
    con_eye_pos=m_program->uniformLocation("eye_position");
    con_box_min=m_program->uniformLocation("box_min");
    con_box_max=m_program->uniformLocation("box_max");
    con_up=m_program->uniformLocation("up");

    con_sample=m_program->uniformLocation("sample");


    color_table[0][0]=QVector4D(0,0,0,0);
    color_table[1][0]=QVector4D(0,0,0,0);
    color_table[2][0]=QVector4D(0,0,0,0);
    color_table[3][0]=QVector4D(0,0,0,0);
    color_table[4][0]=QVector4D(0,0,0,0);
    color_table[5][0]=QVector4D(0,0,0,0);
    color_table[6][0]=QVector4D(0,0,0,0);
    color_table[7][0]=QVector4D(0,0,0,0);
    color_table[8][0]=QVector4D(0,0,0,0);
    color_table[9][0]=QVector4D(0,0,0,0);

    for(int i=1;i<256;i++){
        color_table[0][i]=QVector4D(1,1,0,1);
        color_table[1][i]=QVector4D(1,1,0,1);
        color_table[2][i]=QVector4D(1,1,0,1);
        color_table[3][i]=QVector4D(1,1,0,1);
        color_table[4][i]=QVector4D(1,1,0,1);
        color_table[5][i]=QVector4D(1,1,0,1);
        color_table[6][i]=QVector4D(1,1,0,1);
        color_table[7][i]=QVector4D(1,1,0,1);
        color_table[8][i]=QVector4D(1,1,0,1);
        color_table[9][i]=QVector4D(1,1,0,1);

    }



    l_t=3;
    con_l_t=m_program->uniformLocation("l_t");



    con_specu=m_program->uniformLocation("Uspecu");
    con_diffu=m_program->uniformLocation("Udiffu");
    con_ambi=m_program->uniformLocation("Uambi");
    con_shin=m_program->uniformLocation("Ushin");

    Load3DFile();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(con_tex,0);
    glBindTexture(GL_TEXTURE_3D,tex_3D);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(con_gradient,1);
    glBindTexture(GL_TEXTURE_3D,gradient);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(con_gradient2,2);
    glBindTexture(GL_TEXTURE_3D,gradient2);




    for(int i=0;i<tf_num;i++){
        char ttt[30];
        sprintf(ttt,"color_table[%d]",i);
        con_color_table[i]=m_program->uniformLocation(ttt);
    }

    glGenTextures(1,&color_tex[0]);
    glBindTexture(GL_TEXTURE_1D,color_tex[0]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[0]);



    glGenTextures(1,&color_tex[1]);
    glBindTexture(GL_TEXTURE_1D,color_tex[1]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[1]);



    glGenTextures(1,&color_tex[2]);
    glBindTexture(GL_TEXTURE_1D,color_tex[2]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[2]);



    glGenTextures(1,&color_tex[3]);
    glBindTexture(GL_TEXTURE_1D,color_tex[3]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[3]);



    glGenTextures(1,&color_tex[4]);
    glBindTexture(GL_TEXTURE_1D,color_tex[4]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[4]);


    glGenTextures(1,&color_tex[5]);
    glBindTexture(GL_TEXTURE_1D,color_tex[5]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[5]);


    glGenTextures(1,&color_tex[6]);
    glBindTexture(GL_TEXTURE_1D,color_tex[6]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[6]);


    glGenTextures(1,&color_tex[7]);
    glBindTexture(GL_TEXTURE_1D,color_tex[7]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[7]);



    glGenTextures(1,&color_tex[8]);
    glBindTexture(GL_TEXTURE_1D,color_tex[8]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[8]);



    glGenTextures(1,&color_tex[9]);
    glBindTexture(GL_TEXTURE_1D,color_tex[9]);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,256,0,GL_RGBA,GL_FLOAT,color_table[9]);


    glActiveTexture(GL_TEXTURE3);
    glUniform1i(con_color_table[0],3);
    glBindTexture(GL_TEXTURE_1D,color_tex[0]);
    glActiveTexture(GL_TEXTURE4);
    glUniform1i(con_color_table[1],4);
    glBindTexture(GL_TEXTURE_1D,color_tex[1]);
    glActiveTexture(GL_TEXTURE5);
    glUniform1i(con_color_table[2],5);
    glBindTexture(GL_TEXTURE_1D,color_tex[2]);
    glActiveTexture(GL_TEXTURE6);
    glUniform1i(con_color_table[3],6);
    glBindTexture(GL_TEXTURE_1D,color_tex[3]);
    glActiveTexture(GL_TEXTURE7);
    glUniform1i(con_color_table[4],7);
    glBindTexture(GL_TEXTURE_1D,color_tex[4]);
    glActiveTexture(GL_TEXTURE8);
    glUniform1i(con_color_table[5],8);
    glBindTexture(GL_TEXTURE_1D,color_tex[5]);
    glActiveTexture(GL_TEXTURE9);
    glUniform1i(con_color_table[6],9);
    glBindTexture(GL_TEXTURE_1D,color_tex[6]);
    glActiveTexture(GL_TEXTURE10);
    glUniform1i(con_color_table[7],10);
    glBindTexture(GL_TEXTURE_1D,color_tex[7]);
    glActiveTexture(GL_TEXTURE11);
    glUniform1i(con_color_table[8],11);
    glBindTexture(GL_TEXTURE_1D,color_tex[8]);
    glActiveTexture(GL_TEXTURE12);
    glUniform1i(con_color_table[9],12);
    glBindTexture(GL_TEXTURE_1D,color_tex[9]);
    glActiveTexture(GL_TEXTURE13);


    for(int i=0;i<10;i++){
        probability[i]=new unsigned char[w*h*d];

        for(int j=0;j<w*h*d;j++){
            if(i==0)probability[i][j]=data[j];
            else probability[i][j]=0;
        }
    }

    for(int i=0;i<10;i++){
        glGenTextures(1,&proba_tex[i]);
        glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

        glTexImage3D(GL_TEXTURE_3D,0,GL_RED,w,h,d,0,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
    }


    for(int i=0;i<10;i++){

        char ttt[30];
        sprintf(ttt,"proba_tex[%d]",i);

        con_proba_tex[i]=m_program->uniformLocation(ttt);

        glActiveTexture(GL_TEXTURE16+i);
        glUniform1i(con_proba_tex[i],16+i);
        glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
    }

    QVector3D t(w,h,d);
    const_box_min=-t.normalized();
    const_box_max=t.normalized();


    background_color=QVector4D(0.95,0.95,0.95,1);
    con_background_color=m_program->uniformLocation("background_color");


    m_program->release();

}

void GLWidget::ReadColorTable(){
    float r,g,b,a;
    FILE *f=fopen("color_table0.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[0][i]=QVector4D(r,g,b,a);
    }
    fclose(f);

    glBindTexture(GL_TEXTURE_1D,color_tex[0]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[0]);


    f=fopen("color_table1.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[1][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[1]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[1]);


    f=fopen("color_table2.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[2][i]=QVector4D(r,g,b,a);
    }
    fclose(f);



    glBindTexture(GL_TEXTURE_1D,color_tex[2]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[2]);


    f=fopen("color_table3.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[3][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[3]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[3]);


    f=fopen("color_table4.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[4][i]=QVector4D(r,g,b,a);
    }
    fclose(f);



    glBindTexture(GL_TEXTURE_1D,color_tex[4]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[4]);

    f=fopen("color_table5.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[5][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[5]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[5]);


    f=fopen("color_table6.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[6][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[6]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[6]);

    f=fopen("color_table7.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[7][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[7]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[7]);




    f=fopen("color_table8.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[8][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[8]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[8]);



    f=fopen("color_table9.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[9][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[9]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[9]);



    f=fopen("color_table10.txt","r");
    for(int i=0;i<256;i++){
        fscanf(f,"%f %f %f %f",&r,&g,&b,&a);
        color_table[10][i]=QVector4D(r,g,b,a);
    }
    fclose(f);


    glBindTexture(GL_TEXTURE_1D,color_tex[10]);
    glTexSubImage1D(GL_TEXTURE_1D,0,0,256,GL_RGBA,GL_FLOAT,color_table[10]);



}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fps++;

    QVector3D box_t(w,h,d*z_scale);
    const_box_min=-box_t.normalized();
    const_box_max=box_t.normalized();
    QVector3D right=cross_product(up,center1-eye_position).normalized();

    box_min=(const_box_min+right*shift.x()+up*shift.y())*scale;
    box_max=(const_box_max+right*shift.x()+up*shift.y())*scale;

    m_program->bind();

    ReadColorTable();
    m_program->setUniformValue(con_eye_pos,eye_position);
    m_program->setUniformValue(con_up,up);
    m_program->setUniformValue(con_box_min,box_min);
    m_program->setUniformValue(con_box_max,box_max);

    m_program->setUniformValue(con_sample,sample/scale);


    m_program->setUniformValue(con_diffu,diffu);
    m_program->setUniformValue(con_ambi,ambi);
    m_program->setUniformValue(con_specu,specu);
    m_program->setUniformValue(con_shin,shin);

    m_program->setUniformValue(con_l_t,l_t);
    m_program->setUniformValue(con_background_color,background_color);

    glBegin(GL_QUADS);
        glVertex3f(-1,-1,1);
        glVertex3f(-1,1,1);
        glVertex3f(1,1,1);
        glVertex3f(1,-1,1);
    glEnd();

    m_program->release();

    glColor3f(1-background_color.x(),1-background_color.y(),1-background_color.z());
    glBegin(GL_QUADS);
        glVertex3f(-1,1,1);
        glVertex3f(-1,0.95,1);
        glVertex3f(-0.95,0.95,1);
        glVertex3f(-0.95,1,1);
    glEnd();

    QPainter painter(this);
    painter.setPen(QColor(255-background_color.x()*255,255-background_color.y()*255,255-background_color.z()*255));
    painter.drawText(30, 30, "FPS: " + p_fps);
    painter.end();

}

void GLWidget::resizeGL(int w, int h)
{
    int t=w<h?w:h;
    this->resize(t,t);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();

    if(m_lastPos.x()<30 && m_lastPos.y()<30){
        QColor t_c;
        t_c.setRgb((QColorDialog::getColor(QColor(int(background_color.x()*255),int(background_color.y()*255),int(background_color.z()*255)))).rgb());
        background_color=QVector4D(float(t_c.red())/255,float(t_c.green())/255,float(t_c.blue())/255,1);
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        QVector3D ttt=cross_product(up,center1-eye_position).normalized();
        eye_position=(eye_position+ttt*dx/2000*EYE).normalized()*EYE;

        ttt=cross_product(up,center1-eye_position).normalized();
        eye_position=(eye_position+up*dy/2000*EYE).normalized()*EYE;

        up=cross_product(center1-eye_position,ttt).normalized();
   }
    else if (event->buttons() & Qt::RightButton) {
        scale+=1.0*(-dy)/40000*EYE;
   }
    else if(event->buttons() & Qt::MiddleButton){
        shift+=QVector2D(-float(dx)/1000/scale,-float(dy)/1000/scale);
    }

//    m_lastPos = event->pos();
    update();
}

void GLWidget::timerEvent(QTimerEvent *event){
    if(tog==0){
        QVector3D ttt=cross_product(up,center1-eye_position).normalized();
        eye_position=(eye_position+ttt*0.005*EYE).normalized()*EYE;
    }

    if(fps_start==-1){
        fps_start=GetTickCount();
    }
    else{
        if(GetTickCount()-fps_start>1000){
            char itoa_t[10];
            p_fps=QString(itoa(fps,itoa_t,10));
            fps=0;
            fps_start=GetTickCount();
        }
    }
    update();
}
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_R){
        tog=abs(tog-1);

    }
    if(event->key()==Qt::Key_0){
        FILE *light=fopen("light.txt","r");
        fscanf(light,"%f %f %f %f",&ambi,&diffu,&specu,&shin);
        fclose(light);
    }
    if(event->key()==Qt::Key_P){
        FILE *position=fopen("position.txt","w");
        fprintf(position,"%f %f %f\n%f %f %f\n%f\n",eye_position.x(),eye_position.y(),eye_position.z(),up.x(),up.y(),up.z(),scale);
        fclose(position);
    }
    if(event->key()==Qt::Key_O){
        FILE *position=fopen("position.txt","r");
        float t_e1,t_e2,t_e3,t_u1,t_u2,t_u3,t_s;
        fscanf(position,"%f %f %f %f %f %f %f",&t_e1,&t_e2,&t_e3,&t_u1,&t_u2,&t_u3,&t_s);
        fclose(position);
        eye_position.setX(t_e1);
        eye_position.setY(t_e2);
        eye_position.setZ(t_e3);
        up.setX(t_u1);
        up.setY(t_u2);
        up.setZ(t_u3);
        scale=t_s;
    }
    if(event->key()==Qt::Key_1){
        ambi+=0.001;
    }
    if(event->key()==Qt::Key_2){
        ambi-=0.001;
        if(ambi<0)ambi=0;
    }

    if(event->key()==Qt::Key_3){
        diffu+=0.001;
    }
    if(event->key()==Qt::Key_4){
        diffu-=0.001;
        if(diffu<0)diffu=0;
    }

    if(event->key()==Qt::Key_5){
        specu+=0.001;
    }
    if(event->key()==Qt::Key_6){
        specu-=0.001;
        if(specu<0)specu=0;
    }

    if(event->key()==Qt::Key_7){
        shin+=0.001;
    }
    if(event->key()==Qt::Key_8){
        shin-=0.001;
        if(shin<0)shin=0;
    }
    if(event->key()==Qt::Key_S){
        l_t++;
    }
    if(event->key()==Qt::Key_A){
        l_t--;
    }
    if(event->key()==Qt::Key_L){
        FILE*f=fopen("save_result.raw","wb");
        fwrite(data2,1,w*h*d,f);
        fclose(f);
    }
//    if(event->key()==Qt::Key_B){

//        uchar4 *screen_shot=new uchar4[this->width()*this->height()];

//        char ffile_name[100];

//        int pw=(file_num-1)%6;
//        int ph=(file_num-1)/6;
//        int ma=73;
//        int ma2=6;
//        glReadPixels(9+(245+ma)*pw,1080-298-(245+ma2)*ph,this->width(),this->height(),GL_RGBA,GL_UNSIGNED_BYTE,screen_shot);
//        sprintf(ffile_name,"C:\\\\Users\\\\JunY\\\\Desktop\\\\Jun\\\\QT\\\\Final_Result\\\\Result_image\\\\em\\\\%d.bmp",file_num);
//        SaveBMPFile(screen_shot,this->width(),this->height(),ffile_name,"sample.bmp");
//        delete [] screen_shot;
//    }


    if(event->key()==Qt::Key_Z){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                for(int x=0;x<w;x++){
                    int cur=z*w*h+y*w+x;
                    for(int i=0;i<10;i++){
                        if(i!=0)probability[i][cur]=0;
                    }
                }
            }
        }
        for(int i=0;i<10;i++){
            glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
            glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
        }
    }

    if(event->key()==Qt::Key_X){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                for(int x=0;x<w;x++){
                    int cur=z*w*h+y*w+x;
                    for(int i=0;i<10;i++){
                        if(i!=1)probability[i][cur]=0;
                    }
                }
            }
        }
        for(int i=0;i<10;i++){
            glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
            glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
        }
    }
    if(event->key()==Qt::Key_C){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                for(int x=0;x<w;x++){
                    int cur=z*w*h+y*w+x;
                    for(int i=0;i<10;i++){
                        if(i!=2)probability[i][cur]=0;
                    }
                }
            }
        }
        for(int i=0;i<10;i++){
            glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
            glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
        }
    }
    if(event->key()==Qt::Key_V){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                for(int x=0;x<w;x++){
                    int cur=z*w*h+y*w+x;
                    for(int i=0;i<10;i++){
                        if(i!=3)probability[i][cur]=0;
                    }
                }
            }
        }
        for(int i=0;i<10;i++){
            glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
            glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
        }
    }
    if(event->key()==Qt::Key_B){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                for(int x=0;x<w;x++){
                    int cur=z*w*h+y*w+x;
                    for(int i=0;i<10;i++){
                        if(i!=4)probability[i][cur]=0;
                    }
                }
            }
        }
        for(int i=0;i<10;i++){
            glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
            glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
        }
    }
    if(event->key()==Qt::Key_N){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                for(int x=0;x<w;x++){
                    int cur=z*w*h+y*w+x;
                    for(int i=0;i<10;i++){
                        if(i!=5)probability[i][cur]=0;
                    }
                }
            }
        }
        for(int i=0;i<10;i++){
            glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
            glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
        }
    }
    if(event->key()==Qt::Key_M){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                for(int x=0;x<w;x++){
                    int cur=z*w*h+y*w+x;
                    for(int i=0;i<10;i++){
                        if(i!=6)probability[i][cur]=0;
                    }
                }
            }
        }
        for(int i=0;i<10;i++){
            glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
            glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
        }
    }
    if(event->key()==Qt::Key_D){
        int max_l=0;
        for(int i=1;i<d-1;i++){
            for(int j=1;j<h-1;j++){
                for(int k=1;k<w-1;k++){
                    if(c_window->check[i][j][k]>max_l)max_l=c_window->check[i][j][k];
                }
            }
        }
        set_probability(max_l+1);
    }
}


void GLWidget::cut_plane(int r){

    for(int j=0;j<10;j++){
        unsigned char *t=new unsigned char[w*h*d];
        for(int i=0;i<w*h*r;i++){
            t[i]=probability[j][i];
        }
        for(int i=w*h*r;i<w*h*d;i++){
            t[i]=0;
        }
        glBindTexture(GL_TEXTURE_3D,proba_tex[j]);
        glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,t);


        delete [] t;
    }
}

void GLWidget::apply_75d_all_map_rf(int max_label){

    char ttt[30];
    int case_n;
    FILE *case_num=fopen("case_number.txt","r");
    fscanf(case_num,"%d",&case_n);
    fclose(case_num);


    if(!QDir("data_set").exists())QDir().mkdir("data_set");


    std::string query;
    query="python RandomForest2.py ";
    query+=itoa(n_map2*3+is_loc,ttt,10);
    query+=" ";
    query+=itoa(max_label,ttt,10);
    query+=" data_set/X_train_75d_all_map data_set/Y_train_75d_all_map";

    query+=" data_set/X_test_75d_all_map";

    FILE * query_string=fopen("query_string.txt","w");
    fprintf(query_string,"%s\n",query.c_str());
    fclose(query_string);

    if(!QDir("rf_log").exists())QDir().mkdir("rf_log");


    query+=" > ";
    query+="rf_log/75d_all_map_";
    query+=itoa(case_n,ttt,10);

    system(query.c_str());

    set_probability(max_label);
    set_histogram();
    update();


}




void GLWidget::set_histogram(){


    int histo[12][256];
    int total_histo[12];
    for(int i=0;i<256;i++){
        for(int j=0;j<12;j++){
            histo[j][i]=0;
            total_histo[j]=0;
        }
    }
    for(int j=0;j<10;j++){
        for(int i=0;i<w*h*d;i++){
            histo[j][probability[j][i]]++;
            total_histo[j]++;
        }
    }

    for(int i=0;i<10;i++){
        char t[100];
        sprintf(t,"histogram%d.txt",i);
        FILE *histogram=fopen(t,"w");
        for(int j=0;j<256;j++){
            if(total_histo[i]==0){
                fprintf(histogram,"0\n");
            }
            else fprintf(histogram,"%f\n",float(histo[i][j])/total_histo[i]);
        }
        fclose(histogram);
    }
}
void GLWidget::set_z_scale(int n){
    z_scale=1+n*0.01;
}


void GLWidget::save_result(QString a){
    unsigned char *tt_data=new unsigned char[w*h*d];
    for(int i=0;i<w*h*d;i++)tt_data[i]=data2[i];
    FILE *f=fopen((a.toStdString()+"_save_result.raw").c_str(),"wb");
    fwrite(tt_data,1,w*h*d,f);
    fclose(f);
    delete [] tt_data;
}

void GLWidget::ambi_change(int a){
    ambi=float(a)/50;
}

void GLWidget::diff_change(int a){
    diffu=float(a)/100;
}

void GLWidget::spec_change(int a){
    specu=float(a)/100;
}
void GLWidget::shin_change(int a){
    shin=a;
}
void GLWidget::l_t_change(int a){
    l_t=a;
}



void GLWidget::set_probability(int max_label){
    FILE *f=fopen("probability","rb");
    for(int z=0;z<d-2;z++){
        for(int y=0;y<h-2;y++){
            for(int x=0;x<w-2;x++){
                int cur=(z+1)*w*h+(y+1)*w+x+1;
                for(int i=0;i<max_label;i++){
                    float t[1];
                    fread(t,1,4,f);
                    probability[i][cur]=t[0]*255;
                }
            }
        }
    }

    fclose(f);
    for(int i=0;i<max_label;i++){
        glBindTexture(GL_TEXTURE_3D,proba_tex[i]);
        glTexSubImage3D(GL_TEXTURE_3D,0,0,0,0,w,h,d,GL_RED,GL_UNSIGNED_BYTE,probability[i]);
    }
}

