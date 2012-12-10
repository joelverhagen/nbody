#ifndef NBODYTHREAD_H
#define NBODYTHREAD_H

#include <QThread>
#include <QQueue>
#include <QVector>
#include <QMutex>
#include <QTime>
#include "basenbody.h"
#include "nbodyni.h"
#include "nbodyref.h"
#include "nbodymg.h"""
#include "nbodypanel.h"

class NBodyThread : public QThread
{
    Q_OBJECT
public:
    explicit NBodyThread(QObject *parent = 0);

    void run();
    void samplePositions(float* output);
    void setIsStepping(bool i);
    int getStepCount();
    int getBodyCount();

public slots:
    void setBodyCount(int b);
    void setTileSize(int t);
    void setUsingTiled(bool u);
    void setUsingGPU(bool u);
    void setUsingOpenCL(bool u);
    void setDualGPU(bool u);
    void setUnroll(bool u);

private:
    bool isStepping;
    float* currentPositions;
    QMutex lock;
    int stepCount;

    int bodyCount;
    int tileSize;
    bool usingTiled;
    bool usingGPU;
    bool usingOpenCL;
    bool useDualGPU;
    bool unroll;
};

#endif // NBODYMODELTHREAD_H
