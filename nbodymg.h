#ifndef NBODYMG_H
#define NBODYMG_H

#include "CL/cl.h"
#include <string>
#include <fstream>
#include <iostream>
#include <malloc.h>
#include "basenbody.h"
using namespace std;

class nbodymg : public basenbody
{
public:
	nbodymg();
	~nbodymg();
	bool setDevice(int,const char*);
	bool setData(float*,float*,float*,int);
	bool setInterval (float);
	bool setTileSize (int);
	bool timeStep(float*);

private:

	bool setContext (int);
	bool load (float*,float*,float*,int,int);

    int scaleratio;

	cl_context* context;	
	cl_device_id* device_id;	
	cl_command_queue* commands;
	cl_program* program;
	cl_kernel* kernel;
	int devicecount;
	int size;
	cl_mem* l_c_p;
	cl_mem* l_c_v;
	cl_mem* e_c_p;
	cl_mem* e_c_v;
	cl_mem* m;
	cl_mem* n_p;
	cl_mem* n_v;

	float* vbuf;

	cl_event** loadevent;

	bool swap;
	int T;

	const char* location;
};

#endif
