#ifndef OPENGLPANEL_H
#define OPENGLPANEL_H

#include <QGLWidget>
#include <gl/GLU.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
using namespace std;

class NBodyPanel : public QGLWidget
{
    Q_OBJECT
public:
    explicit NBodyPanel(QWidget *parent = 0);
    
public slots:
    void setBodyCount(int n);
    void setPositions(float* positions);

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    int bodyCount;
    float* positions;

};

#endif // OPENGLPANEL_H
