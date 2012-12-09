#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    sample(NULL)
{
    ui->setupUi(this);

    connect(&thread, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(&sampleTimer, SIGNAL(timeout()), this, SLOT(samplePositions()));
    connect(&counterTimer, SIGNAL(timeout()), this, SLOT(reportCounts()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::startSimulation()
{
    // feed the UI values into the thread (settings for the simulation)
    thread.setBodyCount(ui->bodyCountSpinBox->value());
    thread.setTileSize(ui->tileSizeSpinBox->value());
    thread.setUsingGPU(ui->useGPUCheckBox->isChecked());
    thread.setUsingOpenCL(ui->useOpenCLCheckBox->isChecked());
    thread.setUsingTiled(ui->useTilesCheckBox->isChecked());

    // start the thread
    thread.start();

    // clear the step counts
    stepCount = 0;
    stepSum = 0;

    // set up the UI
    ui->nbodyPanel->setBodyCount(thread.getBodyCount());
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->timeStepOutput->clear();
    ui->averageLineEdit->clear();

    // set up the sample buffer
    delete sample;
    sample = new float[4 * thread.getBodyCount()];

    // sample the simulation for display 60 times a second
    sampleTimer.start(1000 / 60);
    counterTimer.start(1000);
}

void Widget::stopSimulation()
{
    thread.setIsStepping(false);
    sampleTimer.stop();
    counterTimer.stop();
}

void Widget::threadFinished()
{
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

void Widget::samplePositions()
{
    thread.samplePositions(sample);
    ui->nbodyPanel->setPositions(sample);
}

void Widget::reportCounts()
{
    int currentCount = thread.getStepCount();
    stepCount++;

    if(stepCount == 1)
    {
        return;
    }

    stepSum += currentCount;
    ui->timeStepOutput->appendPlainText(QString::number(currentCount));
    ui->averageLineEdit->setText(QString().sprintf("%.2f", stepSum / (double) (stepCount - 1)));

    // stop after 20 seconds
    if(stepCount == 21)
    {
        ui->stopButton->click();
    }
}
