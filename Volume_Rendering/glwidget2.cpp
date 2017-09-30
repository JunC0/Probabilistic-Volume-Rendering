///////////////////////////////////////////////////////////
//JunYoung Choi
//juny0603@gmail.com
//Ulsan National Institute of Science and Technology(UNIST)
///////////////////////////////////////////////////////////

#include "glwidget2.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>
#include <QPainter>
#include <QtGui>
#include <QDockWidget>
#include <QColorDialog>

#include <window.h>

int cur_z=0;


GLWidget2::GLWidget2(int num,Window *p,QWidget *parent)
    : QWidget(parent)
{

    label_number=num;
    c_window=p;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    histogram_scale=1;
    background.load("back.png");

    float xx,yy,rr,gg,bb;
    char t[30];
    sprintf(t,"save_condition%d.txt",label_number);
    FILE *f=fopen(t,"r");
    if(f==NULL){
        f=fopen(t,"w");
        fprintf(f,"0 0 0 0 0\n1 1 1 1 1\n-1 -1 -1 -1 -1");
        fclose(f);
        f=fopen(t,"r");
    }

    while(1){
        fscanf(f,"%f %f %f %f %f",&xx,&yy,&rr,&gg,&bb);
        if(xx==-1)break;
        points.push_back(QVector2D(xx,yy));   //resolution
        colors.push_back(QVector3D(rr,gg,bb));
    }
    fclose(f);

    creat_delete=0;
    QWidget::setFocusPolicy(Qt::ClickFocus);


    if(label_number==0) label_color=QVector3D(0,0,0);

    if(num==0)label_color=QVector3D(1,0,0);
    if(num==1)label_color=QVector3D(0,0,1);
    if(num==2)label_color=QVector3D(0,1,0);
    if(num==3)label_color=QVector3D(1,1,0);
    if(num==4)label_color=QVector3D(0,1,1);
}

GLWidget2::~GLWidget2(){
    char t[30];
    sprintf(t,"save_condition%d.txt",label_number);
    FILE *f=fopen(t,"w");
     int size=points.size();
     for(int i=0;i<size;i++){
         fprintf(f,"%f %f %f %f %f\n"
                 ,points[i].x(),points[i].y()
                 ,colors[i].x(),colors[i].y(),colors[i].z());
     }
     fprintf(f,"-1 -1 -1 -1 -1");
     fclose(f);
}
void GLWidget2::resizeEvent(QResizeEvent *event){
    int w=event->size().width();
    int h=event->size().height();
    this->resize(w,h);
}

QSize GLWidget2::minimumSizeHint() const
{
    return QSize(800, 800);
}

QSize GLWidget2::sizeHint() const
{
    return QSize(800, 800);
}

void GLWidget2::paintEvent(QPaintEvent *)
{

    int size=points.size();
    pen_size=this->width()*0.010;

    TF_start=QVector2D(this->width()*0.055,this->height()*0.88);
    TF_stop=QVector2D(this->width()*0.92,this->height()*0.15);


     QPainter painter(this);
     painter.setRenderHint(QPainter::HighQualityAntialiasing,true);

     painter.drawPixmap(0,0,this->width(),this->height(),background);//resolution



     char his_t[20];
     sprintf(his_t,"histogram%d.txt",label_number);
     FILE *histogram=fopen(his_t,"r");

     if(histogram!=NULL){
         int start_index=0,stop_index=1;
         for(int i=0;i<256;i++){
             float t_num;
             fscanf(histogram,"%f",&t_num);
             QPen histo_pen;
             histo_pen.setWidthF((TF_stop.x()-TF_start.x())/256);
             QPointF start_t,stop_t;
             start_t=QPointF(TF_start.x()+i*(TF_stop.x()-TF_start.x())/256,TF_start.y());
             stop_t=QPointF(TF_start.x()+i*(TF_stop.x()-TF_start.x())/256,(TF_start.y()+t_num*(TF_stop.y()-TF_start.y())*histogram_scale));


             if(float(i)/256>points[stop_index].x())stop_index++,start_index++;

             QVector3D t_color;
             float t_a=((float(i)/256-points[start_index].x())*points[stop_index].y()+(points[stop_index].x()-float(i)/256)*points[start_index].y())/(points[stop_index].x()-points[start_index].x());
             t_a=t_a*3;
             if(t_a>1)t_a=1;
             t_color=((float(i)/256-points[start_index].x())*colors[stop_index]+(points[stop_index].x()-float(i)/256)*colors[start_index])/(points[stop_index].x()-points[start_index].x());
             histo_pen.setColor(QColor(t_color.x()*255*t_a,t_color.y()*255*t_a,t_color.z()*255*t_a));
             painter.setPen(histo_pen);
             painter.drawLine(start_t,stop_t);
         }
         fclose(histogram);
     }



     char tttt[30];
     sprintf(tttt,"color_table%d.txt",label_number);
     FILE *f=fopen(tttt,"w");

     float a=points[0].y();
     float r=colors[0].x();
     float g=colors[0].y();
     float b=colors[0].z();
     fprintf(f,"%f %f %f %f\n",r,g,b,a);
     for(int i=0;i<size-1;i++){
         float a1=points[i].y();
         float a2=points[i+1].y();
         float r1=colors[i].x();
         float r2=colors[i+1].x();
         float g1=colors[i].y();
         float g2=colors[i+1].y();
         float b1=colors[i].z();
         float b2=colors[i+1].z();
         int x1=int(points[i].x()*255);
         int x2=int(points[i+1].x()*255);

         for(int j=x1+1;j<=x2;j++){

             float rr1=(r2-r1)/(x2-x1)*(j-x1)+r1;
             float gg1=(g2-g1)/(x2-x1)*(j-x1)+g1;
             float bb1=(b2-b1)/(x2-x1)*(j-x1)+b1;
             float aa1=(a2-a1)/(x2-x1)*(j-x1)+a1;
             float ta=aa1;
             r=rr1;
             g=gg1;
             b=bb1;
             a=ta;
             fprintf(f,"%f %f %f %f\n",r,g,b,a);
         }
     }

     fclose(f);



     QPen t_pen;
     t_pen.setWidth(pen_size*2);
     t_pen.setColor(QColor(int(colors[0].x()*255),int(colors[0].y()*255),int(colors[0].z()*255)));
     painter.setPen(t_pen);
     painter.drawPoint(TF_start.x()+points[0].x()*(TF_stop.x()-TF_start.x()),
             TF_start.y()+points[0].y()*(TF_stop.y()-TF_start.y()));

     for(int i=1;i<size;i++){
         painter.setPen(Qt::NoPen);
         QPoint start_t,stop_t;
         QColor start_c,stop_c;
         start_c=QColor(int(colors[i-1].x()*255),int(colors[i-1].y()*255),int(colors[i-1].z()*255));
         stop_c=QColor(int(colors[i].x()*255),int(colors[i].y()*255),int(colors[i].z()*255));

         start_t.setX(int(TF_start.x()+points[i-1].x()*(TF_stop.x()-TF_start.x())));
         start_t.setY(int(TF_start.y()+points[i-1].y()*(TF_stop.y()-TF_start.y())));
         stop_t.setX(int(TF_start.x()+points[i].x()*(TF_stop.x()-TF_start.x())));
         stop_t.setY(int(TF_start.y()+points[i].y()*(TF_stop.y()-TF_start.y())));

         QLinearGradient gradient(start_t,stop_t);
         gradient.setColorAt(0.0,start_c);
         gradient.setColorAt(1.0,stop_c);
         painter.setBrush(gradient);
         QPoint tt[4];
         tt[0]=start_t-QPoint(0,1);
         tt[1]=start_t+QPoint(0,1);
         tt[2]=stop_t+QPoint(0,1);
         tt[3]=stop_t-QPoint(0,1);

         painter.drawPolygon(tt,4);

         t_pen.setWidth(pen_size*2);
         t_pen.setColor(stop_c);
         painter.setBrush(Qt::NoBrush);
         painter.setPen(t_pen);
         painter.drawPoint(stop_t);
     }




        t_pen.setColor(QColor(label_color.x()*255,label_color.y()*255,label_color.z()*255));
        t_pen.setWidth(this->height()*0.06);
        painter.setPen(t_pen);
        painter.drawPoint(this->height()*0.03,this->height()*0.97);


}


void GLWidget2::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    button=event->button();

    QPoint t_p;

    if(lastPos.x()>=0 && lastPos.x()<this->height()*0.06 && lastPos.y()>=this->height()*0.94 && lastPos.y()<this->height()){
        QColor t;
        t.setRgb((QColorDialog::getColor(QColor(label_color.x()*255,label_color.y()*255,label_color.z()*255))).rgb());
        label_color.setX(float(t.red())/255);
        label_color.setY(float(t.green())/255);
        label_color.setZ(float(t.blue())/255);
        emit color_change(float(t.red())/255,float(t.green())/255,float(t.blue())/255);
    }

    button=event->button();
    if(event->button()==Qt::MidButton){
        int size=points.size();
        for(int i=0;i<size;i++){
            t_p=QPoint(int(TF_start.x()+points[i].x()*(TF_stop.x()-TF_start.x())),
                    int(TF_start.y()+points[i].y()*(TF_stop.y()-TF_start.y())));
            if((lastPos-t_p).x()<pen_size && (lastPos-t_p).x()>-pen_size &&(lastPos-t_p).y()<pen_size && (lastPos-t_p).y()>-pen_size ){
                QColor t_c;
                t_c.setRgb((QColorDialog::getColor(QColor(int(colors[i].x()*255),int(colors[i].y()*255),int(colors[i].z()*255)))).rgb());
                colors[i]=QVector3D(float(t_c.red())/255,float(t_c.green())/255,float(t_c.blue())/255);
                break;
            }
        }
    }
    else if(event->button() == Qt::RightButton){

        if(creat_delete==1){
            int size=points.size();
            for(int i=1;i<size-1;i++){
                t_p=QPoint(int(TF_start.x()+points[i].x()*(TF_stop.x()-TF_start.x())),
                        int(TF_start.y()+points[i].y()*(TF_stop.y()-TF_start.y())));

                if((lastPos-t_p).x()<pen_size && (lastPos-t_p).x()>-pen_size &&(lastPos-t_p).y()<pen_size && (lastPos-t_p).y()>-pen_size ){
                    points.remove(i);
                    colors.remove(i);
                    break;
                }
            }
        }
        else{
            if(lastPos.x()>TF_start.x() && lastPos.x()<TF_stop.x() && lastPos.y()<=TF_start.y() && lastPos.y()>=TF_stop.y()){//resolution
                int size=points.size();
                for(int i=0;i<size;i++){
                    t_p=QPoint(int(TF_start.x()+points[i].x()*(TF_stop.x()-TF_start.x())),
                            int(TF_start.y()+points[i].y()*(TF_stop.y()-TF_start.y())));

                    if(t_p.x()>lastPos.x()){
                        points.insert(i,QVector2D((lastPos.x()-TF_start.x())/(TF_stop.x()-TF_start.x()),
                                                  (lastPos.y()-TF_start.y())/(TF_stop.y()-TF_start.y())));
                        colors.insert(i,QVector3D((colors[i-1]+colors[i])/2));
                        break;
                    }
                }
            }
        }
    }
    else if(event->button() == Qt::LeftButton){
        int size=points.size();
        for(int i=0;i<size;i++){
            t_p=QPoint(int(TF_start.x()+points[i].x()*(TF_stop.x()-TF_start.x())),
                    int(TF_start.y()+points[i].y()*(TF_stop.y()-TF_start.y())));
            if((lastPos-t_p).x()<pen_size && (lastPos-t_p).x()>-pen_size &&(lastPos-t_p).y()<pen_size && (lastPos-t_p).y()>-pen_size ){
                num=i;
                break;
            }
        num=-1;
        }

    }
    update();

}

void GLWidget2::mouseMoveEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if (button== Qt::LeftButton) {
        if(num!=-1){
            if(lastPos.x()<=TF_start.x())lastPos.setX(TF_start.x()+1); //resolution
            if(lastPos.x()>=TF_stop.x())lastPos.setX(TF_stop.x()-1);//resolution
            if(lastPos.y()>TF_start.y())lastPos.setY(TF_start.y());//resolution
            if(lastPos.y()<TF_stop.y())lastPos.setY(TF_stop.y());//resolution
            int size=points.size();
            if(num==0 || num==size-1){
                points[num].setY((lastPos.y()-TF_start.y())/(TF_stop.y()-TF_start.y()));
            }
            else{
                for(int i=0;i<size;i++){
                    QPoint t_p=QPoint(int(TF_start.x()+points[i].x()*(TF_stop.x()-TF_start.x())),
                            int(TF_start.y()+points[i].y()*(TF_stop.y()-TF_start.y())));
                    if(t_p.x()>lastPos.x()){
                        points.remove(num);
                        QVector3D t=colors[num];
                        colors.remove(num);
                        if(num<i){
                            points.insert(i-1,QVector2D((lastPos.x()-TF_start.x())/(TF_stop.x()-TF_start.x()),
                                                        (lastPos.y()-TF_start.y())/(TF_stop.y()-TF_start.y())));
                            colors.insert(i-1,t);
                            num=i-1;
                        }
                        else{
                            points.insert(i,QVector2D((lastPos.x()-TF_start.x())/(TF_stop.x()-TF_start.x()),
                                                      (lastPos.y()-TF_start.y())/(TF_stop.y()-TF_start.y())));
                            colors.insert(i,t);
                            num=i;
                        }
                        break;
                    }
                }
            }
        }

   }
    else if (button== Qt::RightButton) {
   }
    update();

}

void GLWidget2::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Insert){
           creat_delete=(creat_delete==0)?1:0;
       }
}


void GLWidget2::set_histogram_scale(int n){
    histogram_scale=n;
    update();
}

void GLWidget2::timerEvent(QTimerEvent *event){

}

