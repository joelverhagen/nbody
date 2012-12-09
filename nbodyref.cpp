// nbody.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include "nbodyref.h"

nbodyref::nbodyref()
{
	position = NULL;
	velocity= NULL;
	mass= NULL;
}

nbodyref::~nbodyref()
{
	delete[] position;
	delete[] velocity;
	delete[] mass;
}

bool nbodyref::setData(float* p, float* v, float* m, int length)
{
	try
	{
		delete position;
		delete velocity;
		delete mass;

		position = new float[length*4];
		velocity = new float[length*4];
		N = length;
		mass = new float [length];

		memcpy(position,p,length*4*sizeof(float));
		memcpy(velocity,v,length*4*sizeof(float));
		memcpy(mass,m,length*sizeof(float));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool nbodyref::setInterval (float interval)
{
	try
	{
		deltaT = interval;
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool nbodyref::timeStep(float* output)
{
	try
	{
		float accel [3];
		float delta [3];
		float* newpos = new float[N * 4];

		for (int i = 0; i < N; i++)
		{
			for (int t = 0; t < 3; t++)
				accel[t] = 0.0f;

			for (int j = 0; j < N; j++)
			{
				float sum = 0.0f;

				for (int t = 0; t < 3; t++)
				{
					delta[t] = position[4 * j + t] - position[4 * i + t];
					sum += delta[t] * delta[t];
				}

				float inversecube = 1.0f / sqrt (sum + 500.0f);
				inversecube = inversecube * inversecube * inversecube;
				float temp = inversecube * mass[j];

				for (int t = 0; t < 3; t++)
					accel[t] += temp * delta[t];
			}

			for (int t = 0; t < 3; t++)
			{
					newpos[i*4+t] = position[i*4+t] + deltaT * velocity[i*4+t] + 0.5f * deltaT * deltaT * accel[t];
					velocity[i*4+t] += deltaT * accel[t];
			}
		}

		delete[] position;
		position = newpos;
		memcpy(output,position,N*4*sizeof(float));
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool nbodyref::setTileSize(int t)
{
    throw exception("setTileSize is not used on nbodyref");
}

bool nbodyref::setDevice(int,const char*)
{
    throw exception("setDevice is not used on nbodyref");
}
