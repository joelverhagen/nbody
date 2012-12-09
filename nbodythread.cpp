#include "nbodythread.h"

NBodyThread::NBodyThread(QObject *parent) :
    QThread(parent),
    currentPositions(NULL),
    stepCount(0),
    bodyCount(5)
{
}

int randInt(int low, int high)
{
    return qrand() % ((high + 1) - low) + low;
}

void NBodyThread::run()
{
    basenbody* model;

    // configure the model object according to the parameters set on this thread
    if(usingOpenCL)
    {
        model = new nbodyni();
        int device = usingGPU ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU;
        usingTiled ? model -> setDevice(device, "kernelt.txt") : model->setDevice(device, "kerneln.txt");
    }
    else
    {
        model = new nbodyref();
    }

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    float* positions = new float[bodyCount * 4];
    float* velocities = new float[bodyCount * 4];
    float* masses = new float[bodyCount];

    for(int i = 0; i < bodyCount; i++)
    {
        positions[4 * i + 0] = randInt(-1000, 1000);
        positions[4 * i + 1] = randInt(-1000, 1000);
        positions[4 * i + 2] = 0;

        velocities[4 * i + 0] = 0;
        velocities[4 * i + 1] = 0;
        velocities[4 * i + 2] = 0;

        masses[i] = randInt(1000000, 5000000);
    }

    if(!model->setData(positions, velocities, masses, bodyCount))
    {
        cerr << "Error: There was a problem when setting the initial body data!" << endl;
        return;
    }

    delete[] positions;
    delete[] velocities;
    delete[] masses;

    if(!model->setInterval(0.005f))
    {
        cerr << "Error: There was a problem when setting the time interval!" << endl;
        return;
    }

    if(usingTiled && usingOpenCL)
    {
        if (!model->setTileSize(tileSize))
        {
            cerr << "Error: There was a problem when setting the Tile Size!" << endl;
            return;
        }
    }

    // set up some space to hold the current positions
    lock.lock();
    currentPositions = new float[bodyCount * 4];
    lock.unlock();

    // clear the body count
    stepCount = 0;

    isStepping = true;
    while(isStepping)
    {
        lock.lock();
        model->timeStep(currentPositions);
        stepCount++;
        lock.unlock();
    }

    // delete that space
    lock.lock();
    delete currentPositions;
    currentPositions = NULL;
    lock.unlock();

    // delete the model
    delete model;
}

// this should not be called while the thread is running
void NBodyThread::setBodyCount(int b)
{
    bodyCount = b;
}

int NBodyThread::getStepCount()
{
    lock.lock();
    int output = stepCount;
    stepCount = 0;
    lock.unlock();

    return output;
}

void NBodyThread::setIsStepping(bool i)
{
    isStepping = i;
}

int NBodyThread::getBodyCount()
{
    return bodyCount;
}

void NBodyThread::samplePositions(float* output)
{
    lock.lock();
    if(currentPositions != NULL)
    {
        memcpy(output, currentPositions, sizeof(float) * 4 * bodyCount);
    }
    lock.unlock();
}

void NBodyThread::setUsingOpenCL(bool u)
{
    usingOpenCL = u;
}

void NBodyThread::setUsingGPU(bool u)
{
    usingGPU = u;
}

void NBodyThread::setTileSize(int t)
{
    tileSize = t;
}

void NBodyThread::setUsingTiled(bool u)
{
    usingTiled = u;
}
