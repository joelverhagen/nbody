#include "nbodypanel.h"

NBodyPanel::NBodyPanel(QWidget *parent) :
    QGLWidget(parent)
{
}

void NBodyPanel::initializeGL()
{

}

void NBodyPanel::resizeGL(int w, int h)
{
    // prevent divide by zero
    if (h == 0)
    {
        h = 1;
    }

    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(45.0f, ((float)w) / h, 1.0f, 10000.0f);
    gluLookAt(0.0, 0.0, -5000.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);

}

void NBodyPanel::paintGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(2.0);

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor3f(0.0f, 1.0f, 1.0f);

    glBegin(GL_POINTS);
    for(int i = 0; i < bodyCount; i++)
    {
        glVertex3f(positions[i * 4], positions[i * 4 + 1], positions[i * 4 + 2]);
    }
    glEnd();

    glFlush();
}

void NBodyPanel::setBodyCount(int n)
{
    bodyCount = n;
}

void NBodyPanel::setPositions(float* p)
{
    positions = p;

    updateGL();
}
