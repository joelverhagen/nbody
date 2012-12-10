// nbodymg.cpp : Defines the entry point for the console application.
//
#include "nbodymg.h"

nbodymg::nbodymg()
{
    swap = false;
    T = 0;
    devicecount = 0;
}

nbodymg::~nbodymg()
{
    if (devicecount > 1)
    {
        delete[] device_id;
        for (int i = 0; i < devicecount; i++)
        {
            clReleaseCommandQueue(commands[i]);
            clReleaseContext(context[i]);
            clReleaseProgram(program[i]);
            clReleaseKernel(kernel[i]);
            clReleaseMemObject(l_c_p[i]);
            clReleaseMemObject(l_c_v[i]);
            clReleaseMemObject(e_c_p[i]);
            clReleaseMemObject(e_c_v[i]);
            clReleaseMemObject(m[i]);
            clReleaseMemObject(n_p[i]);
            clReleaseMemObject(n_v[i]);
        }

        delete[] context;
        delete[] commands;
        delete[] program;
        delete[] kernel;
        delete[] l_c_p;
        delete[] l_c_v;
        delete[] e_c_p;
        delete[] e_c_v;
        delete[] m;
        delete[] n_p;
        delete[] n_v;
        delete[] vbuf;
    }
}

bool nbodymg::setDevice(int device_type, const char* file_location)
{
    if (devicecount > 1)
    {
        delete[] device_id;
        for (int i = 0; i < devicecount; i++)
        {
            clReleaseCommandQueue(commands[i]);
            clReleaseContext(context[i]);
            clReleaseProgram(program[i]);
            clReleaseKernel(kernel[i]);
        }

        delete[] context;
        delete[] commands;
        delete[] program;
        delete[] kernel;
    }

    cl_int err;
    cl_uint pcount;
    location = file_location;

    err = clGetPlatformIDs(0, NULL, &pcount);
    if (err != CL_SUCCESS || pcount < 1)
    {
        cerr << "Error: Failed to find a platform!" << endl;
        return false;
    }

    cl_platform_id* platform = new cl_platform_id[pcount];
    err = clGetPlatformIDs(pcount, platform, NULL);
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to find a platform!" << endl;
        return false;
    }

    int total = 0;
    cl_uint tempcount = 0;

    for (int i = 0; i < pcount; i++)
    {
        err = clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, 0, NULL, &tempcount);
        if (err != CL_SUCCESS)
            continue;
        else
            total += tempcount;
    }

    device_id = new cl_device_id[total];
    commands = new cl_command_queue[total];
    context = new cl_context[total];
    int idx = 0;

    for (int i = 0; i < pcount; i++)
    {
        err = clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, 0, NULL, &tempcount);
        if (err != CL_SUCCESS)
            continue;
        else
        {
            cl_device_id* temp_id = new cl_device_id[tempcount];
            clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_GPU, tempcount, temp_id, NULL);

            for (int j = 0; j < tempcount; j++)
            {
                device_id[idx] = temp_id[j];
                idx++;
            }

            delete [] temp_id;
        }
    }

    devicecount = 2;

    context = new cl_context[devicecount];
    commands = new cl_command_queue[devicecount];
    program = new cl_program[devicecount];
    kernel = new cl_kernel[devicecount];

    for (int i = 0; i < devicecount; i++)
    {
        if (!setContext(i))
            return false;
    }

    return true;
}

bool nbodymg::setContext (int idx)
{
    cl_int err;

    context[idx] = clCreateContext(0, 1, &(device_id[idx]), NULL, NULL, &err);
    if (!context)
    {
        cerr << "Error: Failed to create a compute context! " << endl;
        return false;
    }

    commands[idx] = clCreateCommandQueue(context[idx], device_id[idx], 0, &err);
    if (!commands)
    {
        cerr << "Error: Failed to create a command commands!" << endl;
        return false;
    }

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen(location, "r");
    if (!fp)
    {
        fprintf(stderr, "Failed to load kernel.\n");
        return false;
    }
    source_str = (char*)malloc(0x100000);
    source_size = fread( source_str, 1, 0x100000, fp);
    fclose( fp );

    program[idx] = clCreateProgramWithSource(context[idx], 1,(const char **) &source_str,(const size_t *) &source_size, &err);

    if (!program)
    {
        cerr << "Error: Failed to create compute program!" << endl;
        return false;
    }

    err = clBuildProgram(program[idx], 1 ,  &(device_id[idx]), NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t len;
        char buffer[2048];
        cerr << "Error: Failed to build program executable!" << endl;
        clGetProgramBuildInfo(program[idx], device_id[idx], CL_PROGRAM_BUILD_LOG,sizeof(buffer), buffer, &len);
        cerr << buffer << endl;
        return false;
    }

    kernel[idx] = clCreateKernel(program[idx], "nbody", &err);
    if (!kernel || err != CL_SUCCESS)
    {
        cerr << "Error: Failed to create compute kernel!" << endl;
        return false;
    }

    return true;
}

bool nbodymg::setData(float* position,float* velocity,float* mass, int length)
{
    loadevent = new cl_event*[2];
    loadevent[0] = new cl_event[4];
    loadevent[1] = new cl_event[4];

    l_c_p = new cl_mem[2];
    l_c_v = new cl_mem[2];
    e_c_p = new cl_mem[2];
    e_c_v = new cl_mem[2];
    m = new cl_mem[2];
    n_p = new cl_mem[2];
    n_v = new cl_mem[2];

    for (int i = 0 ; i < devicecount; i++)
    {
        if (!load(position, velocity,mass,length,i))
            return false;
    }

    for (int i = 0; i < devicecount; i++)
        clFlush(commands[i]);

    delete [] loadevent[0];
    delete [] loadevent[1];
    delete [] loadevent;

    vbuf = new float[size*4];
    return true;
}

bool nbodymg::load (float* position,float* velocity,float* mass, int length, int id)
{
    cl_int err;
    size = length;

    int llen = length / 2;
    int elen = length - llen;

    l_c_p[id] = clCreateBuffer(context[id], CL_MEM_READ_WRITE, (id == 0 ? llen : elen) * sizeof(cl_float3), NULL, &err);
    l_c_v[id] = clCreateBuffer(context[id], CL_MEM_READ_WRITE, (id == 0 ? llen : elen) * sizeof(cl_float3), NULL, &err);
    e_c_p[id] = clCreateBuffer(context[id], CL_MEM_READ_WRITE, (id == 1 ? llen : elen) * sizeof(cl_float3), NULL, &err);
    e_c_v[id] = clCreateBuffer(context[id], CL_MEM_READ_WRITE, (id == 1 ? llen : elen) * sizeof(cl_float3), NULL, &err);
    m[id] = clCreateBuffer(context[id], CL_MEM_READ_ONLY, length * sizeof(float), NULL, &err);
    n_p[id] = clCreateBuffer(context[id], CL_MEM_READ_WRITE, (id == 0 ? llen : elen) * sizeof(cl_float3), NULL, &err);
    n_v[id] = clCreateBuffer(context[id], CL_MEM_READ_WRITE, (id == 0 ? llen : elen) * sizeof(cl_float3), NULL, &err);

    err = clEnqueueWriteBuffer(commands[id], l_c_p[id], CL_TRUE, 0, sizeof(cl_float3) * (id == 0 ? llen : elen), (id == 0 ? position : (position+llen * 4)), 0, NULL, &(loadevent[id][0]));

    if (err != CL_SUCCESS)
    {
        cerr << "00 Error: Failed to write to source array!" << endl;
        return false;
    }

    err = clEnqueueWriteBuffer(commands[id], l_c_v[id], CL_TRUE, 0, sizeof(cl_float3) * (id == 0 ? llen : elen), (id == 0 ? velocity : (velocity+llen * 4)), 1, loadevent[id], &(loadevent[id][1]));

    if (err != CL_SUCCESS)
    {
        cerr << "01 Error: Failed to write to source array!" << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 0, sizeof(cl_mem), &(l_c_p[id]));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 1, sizeof(cl_mem), &(l_c_v[id]));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clEnqueueWriteBuffer(commands[id], e_c_p[id], CL_TRUE, 0, sizeof(cl_float3) * (id == 0 ? elen : llen), (id == 1 ? position : (position+llen * 4)), 2,loadevent[id], &(loadevent[id][2]));

    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to write to source array!" << endl;
        return false;
    }

    err = clEnqueueWriteBuffer(commands[id], e_c_v[id], CL_TRUE, 0, sizeof(cl_float3) * (id == 0 ? elen : llen), (id == 1 ? velocity: (velocity+llen * 4)), 3,loadevent[id], &(loadevent[id][3]));

    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to write to source array!" << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 2, sizeof(cl_mem), &(e_c_p[id]));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 3, sizeof(cl_mem), &(e_c_v[id]));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clEnqueueWriteBuffer(commands[id], m[id], CL_TRUE, 0, sizeof(float) * length, mass, 4, loadevent[id], NULL);
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to write to source array!" << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 4, sizeof(cl_mem), &(m[id]));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 5, sizeof(cl_mem), &(n_p[id]));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 6, sizeof(cl_mem), &(n_v[id]));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 8, sizeof(int), (id == 0 ? &llen : &elen));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 9, sizeof(int), (id == 0 ? &elen : &llen));
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    err = clSetKernelArg(kernel[id], 10, sizeof(int), &id);
    if (err != CL_SUCCESS)
    {
        cerr << "Error: Failed to set kernel arguments! " << err << endl;
        return false;
    }

    return true;
}

bool nbodymg::setInterval (float deltaT)
{
    cl_int err;

    for (int i = 0; i < devicecount; i++)
    {
        err = clSetKernelArg(kernel[i], 7, sizeof(float), &deltaT);
        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to set kernel arguments! " << err << endl;
            return false;
        }
    }

    return true;
}

bool nbodymg::setTileSize (int tile = 0)
{
    return false;
}

bool nbodymg::timeStep(float* output)
{
    size_t globall = size/2;
    size_t globale = size - globall;
    size_t* local;

    if (T == 0 )
        local = NULL;
    else
    {
        local = new size_t;
        *local = T;
    }

    for (int i = 0; i < 2; i++)
    {
        if (clEnqueueNDRangeKernel(commands[i], kernel[i], 1, NULL, (i == 0 ? &globall : &globale),local,0, NULL,NULL) != CL_SUCCESS)
        {
            cerr << "Error: Failed to execute kernel! " << endl;
            return false;
        }
    }

    for (int i = 0; i < 2; i++)
        clFlush(commands[i]);

    for (int i = 0; i < 2; i++)
    {
        cl_int err;

        err = clEnqueueReadBuffer( commands[i], (swap ? l_c_p[i] : n_p[i]) ,CL_TRUE, 0, (i == 0 ? globall : globale) * sizeof(cl_float3), (i == 0 ? output : (output + globall * 4)),0,NULL, NULL);
        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to read output array! " <<  err << endl;
            return false;
        }

        err = clEnqueueReadBuffer( commands[i], (swap ? l_c_v[i] : n_v[i]) ,CL_TRUE, 0, (i == 0 ? globall : globale) * sizeof(cl_float3), (i == 0 ? vbuf : (vbuf + globall * 4)), 0,NULL,NULL);
        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to read output array! " <<  err << endl;
            return false;
        }
    }

    for (int i = 0; i < 2; i++)
        clFlush(commands[i]);

    for (int i = 0; i < 2; i++)
    {
        cl_int err;
        err = clEnqueueWriteBuffer(commands[i], e_c_p[i], CL_TRUE, 0, sizeof(cl_float3) * (i == 0 ? globale : globall), (i == 0 ? (output + globall * 4) : output), 0,NULL, NULL);

        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to write to source array!" << endl;
            return false;
        }

        err = clEnqueueWriteBuffer(commands[i], e_c_v[i], CL_TRUE, 0, sizeof(cl_float3) * (i == 0 ? globale : globall), (i == 0 ? (vbuf + globall * 4) : vbuf), 0,NULL, NULL);

        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to write to source array!" << endl;
            return false;
        }
    }

    for (int i = 0; i < 2; i++)
        clFlush(commands[i]);

    for (int i = 0; i < 2; i++)
    {
        cl_int err;
        err = clSetKernelArg(kernel[i], 5, sizeof(cl_mem), &(swap ? n_p[i] : l_c_p[i]));
        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to set kernel arguments! " << err << endl;
            return false;
        }

        err = clSetKernelArg(kernel[i], 0, sizeof(cl_mem),  &(swap ? l_c_p[i] : n_p[i]));
        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to set kernel arguments! " << err << endl;
            return false;
        }

        err = clSetKernelArg(kernel[i], 1, sizeof(cl_mem),  &(swap ? l_c_v[i] : n_v[i]));
        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to set kernel arguments! " << err << endl;
            return false;
        }

        err = clSetKernelArg(kernel[i], 6, sizeof(cl_mem), &(swap ? n_v[i] : l_c_v[i]));
        if (err != CL_SUCCESS)
        {
            cerr << "Error: Failed to set kernel arguments! " << err << endl;
            return false;
        }
    }

    swap = !swap;
    delete local;
    return true;
}

