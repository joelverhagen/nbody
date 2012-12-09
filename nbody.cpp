// nbody.cpp : Defines the entry point for the console application.
//
#include "nbody.h"

nbody::nbody()
{
	swap = false;
}

nbody::~nbody()
{
	clReleaseMemObject(c_p);
	clReleaseMemObject(c_v);
	clReleaseMemObject(m);
	clReleaseMemObject(n_p);
	clReleaseMemObject(n_v);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(commands);
	clReleaseContext(context);
}

bool nbody::setDevice(int device_type, const char* file_location)
{
	cl_int err;
	err = clGetPlatformIDs(1, &platform, NULL);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to find a platform!" << endl;
		return false;
	}

	// Get a device of the appropriate type
	device_id = (cl_device_id*)malloc(sizeof(cl_device_id)); // compute device id

	err = clGetDeviceIDs(platform, device_type, 1, device_id, NULL);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to create a device group!" << endl;
		return false;
	}

	context = clCreateContext(0, 1, device_id, NULL, NULL, &err);
	if (!context) 
	{
		cerr << "Error: Failed to create a compute context!" << endl;
		return false;
	}

	commands = clCreateCommandQueue(context, *device_id, 0, &err);
	if (!commands) 
	{
		cerr << "Error: Failed to create a command commands!" << endl;
		return false;
	}
  
	FILE *fp;
	char *source_str;
	size_t source_size;
 
	fp = fopen(file_location, "r");
	if (!fp) 
	{
		fprintf(stderr, "Failed to load kernel.\n");
		false;
	}
	source_str = (char*)malloc(0x100000);
	source_size = fread( source_str, 1, 0x100000, fp);
	fclose( fp );

	program = clCreateProgramWithSource(context, 1,(const char **) &source_str,(const size_t *) &source_size, &err);
	
	if (!program) 
	{
		cerr << "Error: Failed to create compute program!" << endl;
		return false;
	}
    
	err = clBuildProgram(program, 1 , device_id, NULL, NULL, NULL);
	if (err != CL_SUCCESS) 
	{
		size_t len;
		char buffer[2048];    
		cerr << "Error: Failed to build program executable!" << endl;
		clGetProgramBuildInfo(program, *device_id, CL_PROGRAM_BUILD_LOG,sizeof(buffer), buffer, &len);
		cerr << buffer << endl;
		return false;
	}
    
	kernel = clCreateKernel(program, "nbody", &err);
	if (!kernel || err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to create compute kernel!" << endl;
		return false;
	}

	return true;
}

bool nbody::setData(float* position,float* velocity,float* mass, int length)
{
	cl_int err;
	size = length;	
	c_p = clCreateBuffer(context, CL_MEM_READ_WRITE, length * 3 * sizeof(float), NULL, &err);
	c_v = clCreateBuffer(context, CL_MEM_READ_WRITE, length * 3 * sizeof(float), NULL, &err);
	m = clCreateBuffer(context, CL_MEM_READ_ONLY, length * sizeof(float), NULL, &err);
	n_p = clCreateBuffer(context, CL_MEM_READ_WRITE, length * 3 * sizeof(float), NULL, &err);
	n_v = clCreateBuffer(context, CL_MEM_READ_WRITE, length * 3 * sizeof(float), NULL, &err);

	err = clEnqueueWriteBuffer(commands, c_p, CL_TRUE, 0, sizeof(float) * 3 * length, position, 0, NULL, NULL);

	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to write to source array!" << endl;
		return false;
	}

	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &c_p);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}  

	err = clEnqueueWriteBuffer(commands, c_v, CL_TRUE, 0, sizeof(float) * 3 * length, velocity, 0, NULL, NULL);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to write to source array!" << endl;
		return false;
	}

	err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &c_v);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}  

	err = clEnqueueWriteBuffer(commands, m, CL_TRUE, 0, sizeof(float) * length, mass, 0, NULL, NULL);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to write to source array!" << endl;
		return false;
	}

	err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &m);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}  

	err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &n_p);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}  
	
	err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &n_v);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}
	
	err = clSetKernelArg(kernel, 6, sizeof(int), &length);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}

	clFlush(commands);

	return true;
}

bool nbody::setInterval (float deltaT)
{
	deltaT;
	cl_int err;

	err = clSetKernelArg(kernel, 5, sizeof(float), &deltaT);
		if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}
		
	return true;
}

bool nbody::timeStep(float* output)
{
	cl_int err;
	
	size_t global = size;
	err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, NULL,0, NULL, NULL);
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to execute kernel!" << endl;
		return false;
	}

	clFlush(commands);

	err = clEnqueueReadBuffer( commands, (swap ? c_p : n_p) ,CL_TRUE, 0, size * 3 * sizeof(float), output, 0, NULL, NULL ); 

	clFlush(commands);

	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to read output array! " <<  err << endl;
		return false;
	}
		
	err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &(swap ? n_p : c_p));
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}  

	err = clSetKernelArg(kernel, 0, sizeof(cl_mem),  &(swap ? c_p : n_p));
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}  

	err = clSetKernelArg(kernel, 1, sizeof(cl_mem),  &(swap ? c_v : n_v));
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	} 

	err = clSetKernelArg(kernel, 4, sizeof(cl_mem), &(swap ? n_v : c_v));
	if (err != CL_SUCCESS) 
	{
		cerr << "Error: Failed to set kernel arguments! " << err << endl;
		return false;
	}  

	swap = !swap;

	return true;
}
