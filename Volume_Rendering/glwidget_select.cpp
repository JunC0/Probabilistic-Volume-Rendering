///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "glwidget_select.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QtWidgets>
#include "window.h"

int n_map=24;
int undo1_check[305][305][256];
int undo2_check[305][305][256];
int brush_size=1;
int first_flag=0;
float *gradient;
int add_loc=3;

unsigned char *data2;
unsigned char *di_data_24_feature_map;
unsigned char *di_data_24_response_map;
unsigned char *di_data_24_sparse_map;

unsigned char *di_data_h0_map;
unsigned char *di_data_h1_map;
unsigned char *di_data_h2_map;


unsigned char *data_buffer;
float *di_data_75;

QVector3D GLWidget_select::cross_product(QVector3D v1,QVector3D v2){
    QVector3D v;
    v.setX(v1.y()*v2.z()-v2.y()*v1.z());
    v.setY(v1.z()*v2.x()-v2.z()*v1.x());
    v.setZ(v1.x()*v2.y()-v2.x()*v1.y());
    return v;
}

GLWidget_select::GLWidget_select(QUrl fileinfo,int xx,int yy,int zz,Window *parent, QWidget *parent1)
    : QOpenGLWidget(parent1){


    c_window=parent;

    cc=0;
    mw=0;
    tolerance=-1;

    for(int i=0;i<305;i++){
        for(int j=0;j<305;j++){
            for(int k=0;k<256;k++){
                c_window->check[i][j][k]=-1;
            }
        }
    }

    zoom=0;
    select_zoom_range=0;
    cur_select_label=0;

    for(int i=1;i<=25;i++)c_window->label_color[i].setX(1),c_window->label_color[i].setY(1),c_window->label_color[i].setZ(0);

    c_window->label_color[0]=QVector3D(1,0,0);
    c_window->label_color[1]=QVector3D(0,0,1);
    c_window->label_color[2]=QVector3D(0,1,0);
    c_window->label_color[3]=QVector3D(1,1,0);
    c_window->label_color[4]=QVector3D(0,1,1);


    axx=xx;
    axy=yy;
    axz=zz;

    slx=0;
    sly=0;
    slz=0;


    scale=1;
    startTimer(100);
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
    t_size=w*h*d;




    if(axx!=0){
        view_start=QPoint(0,0);
        view_end=QPoint(h,d);
        window_rate_h=float(d)/h;
        window_rate_w=float(h)/d;
    }
    if(axy!=0){
        view_start=QPoint(0,0);
        view_end=QPoint(w,d);
        window_rate_h=float(d)/w;
        window_rate_w=float(w)/d;
    }
    if(axz!=0){
        view_start=QPoint(0,0);
        view_end=QPoint(w,h);
        window_rate_h=float(h)/w;
        window_rate_w=float(w)/h;
    }

    if(window_rate_w<1){
        size_w=400;
        size_h=size_w*window_rate_h;
    }
    else{
        size_w=400;
        size_h=400*window_rate_h;
    }

    data=new unsigned char[w*h*d];
    FILE * f=fopen(filename.c_str(),"rb");
    fread(data,1,w*h*d,f);
    fclose(f);


    if(first_flag==0){

        float t1[150];
        gradient=new float[t_size];
        for(int i=0;i<t_size;i++){
            gradient[i]=0;
        }
        float max_gradient=0;

        for(int i=1;i<d-1;i++){
            for(int j=1;j<h-1;j++){
                for(int k=1;k<w-1;k++){
                    int cur=i*w*h+j*w+k;
                    int xm1=cur-1;
                    int xp1=cur+1;
                    int ym1=cur-w;
                    int yp1=cur+w;
                    int zm1=cur-w*h;
                    int zp1=cur+w*h;

                    gradient[cur]=
                            sqrt(float((data[xp1]-data[xm1])*(data[xp1]-data[xm1])
                                       +(data[yp1]-data[ym1])*(data[yp1]-data[ym1])
                                       +(data[zp1]-data[zm1])*(data[zp1]-data[zm1])));

                    if(gradient[cur]>max_gradient)max_gradient=gradient[cur];

                }
            }
        }
        if(max_gradient!=0){
            for(int i=0;i<t_size;i++){
                gradient[i]=gradient[i]/max_gradient;
            }
        }




        FILE * X_test_set_75d_all_map=fopen("data_set\\X_test_75d_all_map","wb");


        di_data_24_feature_map=new unsigned char[w*h*d*n_map];
        di_data_24_response_map=new unsigned char[w*h*d*n_map];
        di_data_24_sparse_map=new unsigned char[w*h*d*n_map];

        FILE *di_24_feature_map=fopen(get_file_location("map1 for 75dimension").c_str(),"rb");
        FILE *di_24_response_map=fopen(get_file_location("map2 for 75dimension").c_str(),"rb");
        FILE *di_24_sparse_map=fopen(get_file_location("map3 for 75dimension").c_str(),"rb");

        fread(di_data_24_feature_map,1,w*h*d*n_map,di_24_feature_map);
        fread(di_data_24_response_map,1,w*h*d*n_map,di_24_response_map);
        fread(di_data_24_sparse_map,1,w*h*d*n_map,di_24_sparse_map);


        for(int i=1;i<d-1;i++){
            for(int j=1;j<h-1;j++){
                for(int k=1;k<w-1;k++){
                    int cur=i*w*h+j*w+k;

                    for(int nm=0;nm<n_map;nm++){
                        t1[nm]=float(di_data_24_feature_map[cur+nm*w*h*d])/256.0;
                    }


                    for(int nm=0;nm<n_map;nm++){
                        t1[nm+n_map]=float(di_data_24_response_map[cur+nm*w*h*d])/256.0;
                    }

                    for(int nm=0;nm<n_map;nm++){
                        t1[nm+n_map*2]=float(di_data_24_sparse_map[cur+nm*w*h*d])/256.0;
                    }

                    t1[n_map*3+0]=float(k)/w;
                    t1[n_map*3+1]=float(j)/h;
                    t1[n_map*3+2]=float(i)/d;
                    fwrite(t1,4,n_map*3+add_loc,X_test_set_75d_all_map);
                }
            }
        }

        fclose(di_24_feature_map);
        fclose(di_24_response_map);
        fclose(di_24_sparse_map);


        fclose(X_test_set_75d_all_map);



        first_flag=1;

    }

    ReadColorTable();
}

GLWidget_select::~GLWidget_select()
{
    cleanup();
}

QSize GLWidget_select::minimumSizeHint() const
{
    return QSize(1600, 1600);
}

QSize GLWidget_select::sizeHint() const
{
    return QSize(1600, 1600);
}



void GLWidget_select::cleanup()
{
    makeCurrent();
    doneCurrent();
}

void GLWidget_select::initializeGL()
{

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget_select::cleanup);

    initializeOpenGLFunctions();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
}

void GLWidget_select::ReadColorTable(){ 
    for(int i=0;i<256;i++){
        color_table[i]=QVector4D(float(i)/255,float(i)/255,float(i)/255,float(i)/255);
    }

}

void GLWidget_select::paintGL()
{

    glClearColor(1, 1, 1,1);
    glLoadIdentity();
    glOrtho(view_start.x(),view_end.x(),view_start.y(),view_end.y(),100,-100);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);

    if(axx!=0){
        for(int z=0;z<d;z++){
            for(int y=0;y<h;y++){
                QVector4D t_c=color_table[data[z*w*h+y*w+slx]];

                if(c_window->check[z][y][slx]==-1)glColor3f(t_c.x(),t_c.y(),t_c.z());
                else glColor3f(c_window->label_color[c_window->check[z][y][slx]].x(),c_window->label_color[c_window->check[z][y][slx]].y(),c_window->label_color[c_window->check[z][y][slx]].z());

                glVertex2f(y,d-z);
                glVertex2f(y+1,d-z);
                glVertex2f(y+1,d-z+1);
                glVertex2f(y,d-z+1);
            }
        }
    }
    if(axy!=0){
        for(int z=0;z<d;z++){
            for(int x=0;x<w;x++){
                QVector4D t_c=color_table[data[z*w*h+sly*w+x]];

                if(c_window->check[z][sly][x]==-1)glColor3f(t_c.x(),t_c.y(),t_c.z());
                else glColor3f(c_window->label_color[c_window->check[z][sly][x]].x(),c_window->label_color[c_window->check[z][sly][x]].y(),c_window->label_color[c_window->check[z][sly][x]].z());

                glVertex2f(x,d-z);
                glVertex2f(x+1,d-z);
                glVertex2f(x+1,d-z+1);
                glVertex2f(x,d-z+1);
            }
        }
    }
    if(axz!=0){
        for(int y=0;y<h;y++){
            for(int x=0;x<w;x++){
                QVector4D t_c=color_table[data[slz*w*h+y*w+x]];

                if(c_window->check[slz][y][x]==-1)glColor3f(t_c.x(),t_c.y(),t_c.z());
                else glColor3f(c_window->label_color[c_window->check[slz][y][x]].x(),c_window->label_color[c_window->check[slz][y][x]].y(),c_window->label_color[c_window->check[slz][y][x]].z());

                glVertex2f(x,h-y);
                glVertex2f(x+1,h-y);
                glVertex2f(x+1,h-y+1);
                glVertex2f(x,h-y+1);
            }
        }
    }
    glEnd();

    if(select_zoom_range==1){
        glColor4f(1,1,1,0.5);
        glBegin(GL_QUADS);
        glVertex2f(zoom_start.x(),zoom_start.y());
        glVertex2f(zoom_start.x(),zoom_end.y());
        glVertex2f(zoom_end.x(),zoom_end.y());
        glVertex2f(zoom_end.x(),zoom_start.y());

        glEnd();
    }

}

void GLWidget_select::resizeGL(int w, int h){
    if(h*window_rate_w<=w){
        size_w=h*window_rate_w;
        size_h=h;
    }
    else{
        size_w=w;
        size_h=w*window_rate_h;
    }

    this->resize(size_w,size_h);

}

void GLWidget_select::mousePressEvent(QMouseEvent *event)
{
    for(int i=0;i<305;i++){
        for(int j=0;j<305;j++){
            for(int k=0;k<256;k++){
                undo2_check[i][j][k]=undo1_check[i][j][k];
                undo1_check[i][j][k]=c_window->check[i][j][k];
            }
        }
    }
    if(event->button()==Qt::LeftButton){
        if(zoom==1){
            zoom_start=QPoint(view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x()),view_start.y()+float(size_h-event->y())/size_h*(view_end.y()-view_start.y()));
            zoom_end=zoom_start;
            select_zoom_range=1;

        }
        else{
            if(axx!=0){
                int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
                int y=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
                for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                    for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                        if(z+i<d && z+i>=0 && y+j<h && y+j>=0){
                            c_window->check[z+i][y+j][slx]=cur_select_label;
                            magicWand(z+i,y+j,slx,data[(z+i)*w*h+(y+j)*w+slx]);
                        }
                    }
                }
            }
            else if(axy!=0){
                int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
                int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
                for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                    for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                        if(z+i<d && z+i>=0 && x+j<w && x+j>=0){
                            c_window->check[z+i][sly][x+j]=cur_select_label;
                            magicWand(z+i,sly,x+j,data[(z+i)*w*h+sly*w+x+j]);
                        }
                    }
                }
            }
            else{
                int y=(h-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
                int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
                for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                    for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                        if(y+i<h && y+i>=0 && x+j<w && x+j>=0){
                            c_window->check[slz][y+i][x+j]=cur_select_label;
                            magicWand(slz,y+i,x+j,data[slz*w*h+(y+i)*w+x+j]);
                        }
                    }
                }

            }
        }
    }
    if(event->button()==Qt::RightButton){
        if(axx!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int y=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && y+j<h && y+j>=0)c_window->check[z+i][y+j][slx]=0;
                }
            }
        }
        else if(axy!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && x+j<w && x+j>=0)c_window->check[z+i][sly][x+j]=0;
                }
            }
        }
        else{
            int y=(h-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(y+i<h && y+i>=0 && x+j<w && x+j>=0)c_window->check[slz][y+i][x+j]=0;
                }
            }

        }
    }
    if(event->button()==Qt::MiddleButton){
        if(axx!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int y=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && y+j<h && y+j>=0)c_window->check[z+i][y+j][slx]=-1;
                }
            }
        }
        else if(axy!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && x+j<w && x+j>=0)c_window->check[z+i][sly][x+j]=-1;
                }
            }
        }
        else{
            int y=(h-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(y+i<h && y+i>=0 && x+j<w && x+j>=0)c_window->check[slz][y+i][x+j]=-1;
                }
            }

        }
    }

    update();
}


void GLWidget_select::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        if(select_zoom_range==1){
           select_zoom_range=0;
           zoom_end=QPoint(view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x()),view_start.y()+float(size_h-event->y())/size_h*(view_end.y()-view_start.y()));

           if(zoom_start.x()>zoom_end.x()){
               int t=zoom_start.x();
               zoom_start.setX(zoom_end.x());
               zoom_end.setX(t);
           }
           if(zoom_start.y()>zoom_end.y()){
               int t=zoom_start.y();
               zoom_start.setY(zoom_end.y());
               zoom_end.setY(t);
           }

           if(zoom_start.x()!=zoom_end.x() && zoom_start.y()!=zoom_end.y()){
               view_start=zoom_start;
               view_end=zoom_end;
           }
        }
    }
    update();
}



void GLWidget_select::mouseMoveEvent(QMouseEvent *event)
{
    setFocus();
    if(event->buttons() & Qt::LeftButton){
        if(select_zoom_range==1){
            zoom_end=QPoint(view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x()),view_start.y()+float(size_h-event->y())/size_h*(view_end.y()-view_start.y()));
        }
        else{
            if(axx!=0){
                int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
                int y=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
                for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                    for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                        if(z+i<d && z+i>=0 && y+j<h && y+j>=0){
                            c_window->check[z+i][y+j][slx]=cur_select_label;
                            magicWand(z+i,y+j,slx,data[(z+i)*w*h+(y+j)*w+slx]);
                        }
                    }
                }
            }
            else if(axy!=0){
                int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
                int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
                for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                    for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                        if(z+i<d && z+i>=0 && x+j<w && x+j>=0){
                            c_window->check[z+i][sly][x+j]=cur_select_label;
                            magicWand(z+i,sly,x+j,data[(z+i)*w*h+sly*w+x+j]);
                        }
                    }
                }
            }
            else{
                int y=(h-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
                int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
                for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                    for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                        if(y+i<h && y+i>=0 && x+j<w && x+j>=0){
                            c_window->check[slz][y+i][x+j]=cur_select_label;
                            magicWand(slz,y+i,x+j,data[slz*w*h+(y+i)*w+x+j]);
                        }
                    }
                }

            }
        }
    }
    if(event->buttons()& Qt::RightButton){
        if(axx!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int y=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && y+j<h && y+j>=0)c_window->check[z+i][y+j][slx]=0;
                }
            }
        }
        else if(axy!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && x+j<w && x+j>=0)c_window->check[z+i][sly][x+j]=0;
                }
            }
        }
        else{
            int y=(h-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(y+i<h && y+i>=0 && x+j<w && x+j>=0)c_window->check[slz][y+i][x+j]=0;
                }
            }

        }
    }
    if(event->buttons()& Qt::MiddleButton){
        if(axx!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int y=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && y+j<h && y+j>=0)c_window->check[z+i][y+j][slx]=-1;
                }
            }
        }
        else if(axy!=0){
            int z=(d-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(z+i<d && z+i>=0 && x+j<w && x+j>=0)c_window->check[z+i][sly][x+j]=-1;
                }
            }
        }
        else{
            int y=(h-view_end.y())+float(event->y())/size_h*(view_end.y()-view_start.y());
            int x=view_start.x()+float(event->x())/size_w*(view_end.x()-view_start.x());
            for(int i=-brush_size/2+1;i<(brush_size+1)/2+1;i++){
                for(int j=-brush_size/2;j<(brush_size+1)/2;j++){
                    if(y+i<h && y+i>=0 && x+j<w && x+j>=0)c_window->check[slz][y+i][x+j]=-1;
                }
            }

        }
    }
    update();
}

void GLWidget_select::timerEvent(QTimerEvent *event){

    update();

}
double tt12[128*128*30];
void GLWidget_select::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control){
        zoom=1;
    }
    if(zoom==1){
        if(event->key()==Qt::Key_Z){
            for(int i=0;i<305;i++){
                for(int j=0;j<305;j++){
                    for(int k=0;k<256;k++){
                        c_window->check[i][j][k]=undo1_check[i][j][k];
                        undo1_check[i][j][k]=undo2_check[i][j][k];
                    }
                }
            }
        }
    }
    if(event->key()==Qt::Key_Escape){
        if(axx!=0){
            view_start=QPoint(0,0);
            view_end=QPoint(h,d);
        }
        if(axy!=0){
            view_start=QPoint(0,0);
            view_end=QPoint(w,d);
        }
        if(axz!=0){
            view_start=QPoint(0,0);
            view_end=QPoint(w,h);
        }

    }
    if(event->key()==Qt::Key_C){
        for(int i=0;i<d;i++){
            for(int j=0;j<h;j++){
                for(int k=0;k<w;k++){
                    if(c_window->check[i][j][k]==0)c_window->check[i][j][k]=-1;
                }
            }
        }
    }
}
void GLWidget_select::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control){
        zoom=0;
    }
}
void GLWidget_select::set_brush_size(int a){
    brush_size=a;
}

void GLWidget_select::apply(bool a){

    int case_n;
    char ttt[30];
    FILE *case_num=fopen("case_number.txt","r");
    fscanf(case_num,"%d",&case_n);
    fclose(case_num);

    std::string file_name="select_log\\\\";
    file_name+=itoa(case_n,ttt,10);
    file_name+="_";
    file_name+=itoa(w,ttt,10);
    file_name+="_";
    file_name+=itoa(h,ttt,10);
    file_name+="_";
    file_name+=itoa(d,ttt,10);


    FILE *check_log=fopen(file_name.c_str(),"w");
    for(int i=0;i<d;i++){
        for(int j=0;j<h;j++){
            for(int k=0;k<w;k++){
                fprintf(check_log,"%d ",c_window->check[i][j][k]);
            }
        }
    }
    fclose(check_log);

    FILE *selected_voxels=fopen("selected_voxels.txt","w");
    int svt=0;
    for(int i=1;i<d-1;i++){
        for(int j=1;j<h-1;j++){
            for(int k=1;k<w-1;k++){
                if(c_window->check[i][j][k]!=-1){
                    svt++;
                }
            }
        }
    }
    fprintf(selected_voxels,"%d\n",svt);
    fclose(selected_voxels);

    FILE * X_train_set_75d_all_map=fopen("data_set\\X_train_75d_all_map","wb");
    FILE * Y_train_set_75d_all_map=fopen("data_set\\Y_train_75d_all_map","wb");


    float t[150];
    for(int i=1;i<d-1;i++){
        for(int j=1;j<h-1;j++){
            for(int k=1;k<w-1;k++){
                int cur=i*w*h+j*w+k;
                if(c_window->check[i][j][k]!=-1){

                    float label[1];
                    label[0]=float(c_window->check[i][j][k]);

                    for(int nm=0;nm<n_map;nm++){
                        t[nm]=float(di_data_24_feature_map[cur+nm*w*h*d])/256.0;
                    }
                    for(int nm=0;nm<n_map;nm++){
                        t[nm+n_map]=float(di_data_24_response_map[cur+nm*w*h*d])/256.0;
                    }
                    for(int nm=0;nm<n_map;nm++){
                        t[nm+n_map*2]=float(di_data_24_sparse_map[cur+nm*w*h*d])/256.0;
                    }
                    t[n_map*3+0]=float(k)/w;
                    t[n_map*3+1]=float(j)/h;
                    t[n_map*3+2]=float(i)/d;
                    fwrite(label,4,1,Y_train_set_75d_all_map);
                    fwrite(t,4,n_map*3+add_loc,X_train_set_75d_all_map);
                }
            }
        }
    }
    fclose(X_train_set_75d_all_map);
    fclose(Y_train_set_75d_all_map);



    int max_l=0;
    for(int i=1;i<d-1;i++){
        for(int j=1;j<h-1;j++){
            for(int k=1;k<w-1;k++){
                if(c_window->check[i][j][k]>max_l)max_l=c_window->check[i][j][k];
            }
        }
    }


    emit sigapply_75d_all_map_rf(max_l+1);

    case_num=fopen("case_number.txt","w");
    fprintf(case_num,"%d",case_n+1);
    fclose(case_num);

    emit sigset_histogram();

}

void GLWidget_select::reset(bool a){
    for(int i=0;i<d;i++){
        for(int j=0;j<h;j++){
            for(int k=0;k<w;k++){
                c_window->check[i][j][k]=-1;
            }
        }
    }
    update();
}

void GLWidget_select::label_change(int n){
    cur_select_label=n;
}
void GLWidget_select::color_change(float r, float g, float b){
    c_window->label_color[cur_select_label].setX(r);
    c_window->label_color[cur_select_label].setY(g);
    c_window->label_color[cur_select_label].setZ(b);
}

void GLWidget_select::o_selection(){
    QUrl filepath = QFileDialog::getOpenFileUrl(this);
    if (!filepath.fileName().isEmpty()){
        std::string ft=(filepath.toString(0)).toStdString();
        std::string ft1;
        for(int i=8;i<ft.size();i++){
            if(ft[i]=='/'){
                ft1+="\\\\";
            }
            else{
                ft1+=ft[i];
            }
        }
        FILE *f=fopen(ft1.c_str(),"r");
        for(int i=0;i<d;i++){
            for(int j=0;j<h;j++){
                for(int k=0;k<w;k++){
                    fscanf(f,"%d ",&c_window->check[i][j][k]);
                }
            }
        }
        fclose(f);
    }
}
void GLWidget_select::s_selection(){

}




void GLWidget_select::magicWand(int z, int y, int x, int intensity){
    int queue_x[1001],queue_y[1001],queue_z[1001];
    int cur_queue=1;
    int front_queue=0;
    int queue_size=1;
    queue_x[0]=x;
    queue_y[0]=y;
    queue_z[0]=z;
    int z2, y2, x2;
    while(queue_size>0){

        z = queue_z[front_queue];
        y = queue_y[front_queue];
        x = queue_x[front_queue];
        front_queue=(front_queue+1)%1000;
        queue_size--;

        z2 = z;
        y2 = y;
        x2 = x+1;
        if (x2<w-2 && c_window->check[z2][y2][x2]==-1 && (abs(data[z2*w*h+y2*w+x2]-intensity)<=tolerance)){
            c_window->check[z2][y2][x2] = cur_select_label;
            if(queue_size<1000){
                queue_size++;
                queue_x[cur_queue]=x2;
                queue_y[cur_queue]=y2;
                queue_z[cur_queue]=z2;
                cur_queue=(cur_queue+1)%1000;
            }
        }

        z2 = z;
        y2 = y;
        x2 = x-1;
        if (x2>=2 && c_window->check[z2][y2][x2]==-1 && (abs(data[z2*w*h+y2*w+x2]-intensity)<=tolerance)){
           c_window->check[z2][y2][x2] = cur_select_label;
            if(queue_size<1000){
                queue_size++;
                queue_x[cur_queue]=x2;
                queue_y[cur_queue]=y2;
                queue_z[cur_queue]=z2;
                cur_queue=(cur_queue+1)%1000;
            }
        }

        z2 = z;
        y2 = y+1;
        x2 = x;
        if (y2<h-2 && c_window->check[z2][y2][x2]==-1 && (abs(data[z2*w*h+y2*w+x2]-intensity)<=tolerance)){
            c_window->check[z2][y2][x2] = cur_select_label;
            if(queue_size<1000){
                queue_size++;
                queue_x[cur_queue]=x2;
                queue_y[cur_queue]=y2;
                queue_z[cur_queue]=z2;
                cur_queue=(cur_queue+1)%1000;
            }
        }

        z2 = z;
        y2 = y-1;
        x2 = x;
        if (y2>=2 && c_window->check[z2][y2][x2]==-1 && (abs(data[z2*w*h+y2*w+x2]-intensity)<=tolerance)){
            c_window->check[z2][y2][x2] = cur_select_label;
            if(queue_size<1000){
                queue_size++;
                queue_x[cur_queue]=x2;
                queue_y[cur_queue]=y2;
                queue_z[cur_queue]=z2;
                cur_queue=(cur_queue+1)%1000;
            }
        }

        z2 = z+1;
        y2 = y;
        x2 = x;
        if (z2<d-2 && c_window->check[z2][y2][x2]==-1 && (abs(data[z2*w*h+y2*w+x2]-intensity)<=tolerance)){
            c_window->check[z2][y2][x2] = cur_select_label;
            if(queue_size<1000){
                queue_size++;
                queue_x[cur_queue]=x2;
                queue_y[cur_queue]=y2;
                queue_z[cur_queue]=z2;
                cur_queue=(cur_queue+1)%1000;
            }

        }

        z2 = z-1;
        y2 = y;
        x2 = x;
        if (z2>=2 && c_window->check[z2][y2][x2]==-1 && (abs(data[z2*w*h+y2*w+x2]-intensity)<=tolerance)){
            c_window->check[z2][y2][x2] = cur_select_label;
            if(queue_size<1000){
                queue_size++;
                queue_x[cur_queue]=x2;
                queue_y[cur_queue]=y2;
                queue_z[cur_queue]=z2;
                cur_queue=(cur_queue+1)%1000;
            }
        }

    }
}
void GLWidget_select::set_tolerance(int to){
    tolerance=to;
}

std::string GLWidget_select::get_file_location(std::string inform){
    QUrl filepath;
    filepath = QFileDialog::getOpenFileUrl(nullptr,QString(inform.c_str()));
    std::string ft=(filepath.toString(0)).toStdString();
    std::string ft1;
    for(int i=8;i<ft.size();i++){
        if(ft[i]=='/'){
            ft1+="\\\\";
        }
        else{
            ft1+=ft[i];
        }
    }
    return ft1;
}
