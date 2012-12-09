#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include "nbodythread.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private:
    Ui::Widget *ui;
    NBodyThread thread;
    QTimer sampleTimer;
    QTimer counterTimer;
    float* sample;
    int stepSum;
    int stepCount;

public slots:
    void startSimulation();
    void stopSimulation();

private slots:
    void threadFinished();
    void samplePositions();
    void reportCounts();

};

#endif // WIDGET_H
