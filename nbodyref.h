#ifndef NBODYREF_H
#define NBODYREF_H

#include <string>
#include <iostream>
#include <cmath>
#include "basenbody.h"
using namespace std;

class nbodyref : public basenbody
{
public:
	nbodyref();
    ~nbodyref();
	bool setData(float*,float*,float*,int);
	bool setInterval (float);
	bool timeStep(float*);
    bool setTileSize (int);
    bool setDevice(int,const char*);

private:
	float* position;
	float* velocity;
	float* mass;
	float deltaT;
	int N;
};

#endif
