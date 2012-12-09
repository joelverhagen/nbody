#include "CL/cl.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class nbody
{
public:
	nbody();
	~nbody();
	bool setDevice(int,const char*);
	bool setData(float*,float*,float*,int);
	bool setInterval (float);
	bool timeStep(float*);

private:
	cl_context context;
	cl_platform_id platform;
	cl_device_id* device_id;
	cl_command_queue commands;
	cl_program program;
	cl_kernel kernel;
	int size;
	cl_mem c_p;
	cl_mem c_v;
	cl_mem m;
	cl_mem n_p;
	cl_mem n_v;
	bool swap;
};