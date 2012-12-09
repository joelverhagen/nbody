#ifndef BASENBODY_H
#define BASENBODY_H

class basenbody
{
public:
    virtual bool setDevice(int,const char*) = 0;
    virtual bool setData(float*, float*, float*, int) = 0;
    virtual bool setInterval (float) = 0;
    virtual bool setTileSize (int) = 0;
    virtual bool timeStep(float*) = 0;
};

#endif // BASENBODY_H
