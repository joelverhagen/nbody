__kernel void nbody (__global float3* c_p,__global float3* c_v,__global float* m,__global float3* n_p,__global float3* n_v,const float dt, const int s,__local float3* tile,__local float* mass)
{
	int g = get_global_id(0);

	if (g < s)
	{
		int l = get_local_id(0);
		int lsize = get_local_size(0);
		int tiles = get_global_size(0)/lsize;

		float3 positionv= c_p[g];
		float3 velocityv= c_v[g];
		float3 delta;
		float3 accel = (float3) (0.0f,0.0f,0.0f);
		
		float invcube;
		for (int b = 0; b < tiles; b++)
		{
			tile[l] = c_p[b * lsize + l];
			mass[l] = m[b * lsize + l];
			barrier(CLK_LOCAL_MEM_FENCE);

			for (int i = 0; i < lsize; i++)
			{
				delta = tile[i] - positionv;
			
				invcube = rsqrt(dot(delta,delta)+ 500.0f);
				invcube *= invcube*invcube;
				invcube *= mass[i];
				accel += invcube * delta;
			}

			barrier(CLK_LOCAL_MEM_FENCE);
		}

		positionv += dt * velocityv + (0.5f * dt * dt) * accel;
		velocityv += dt * accel;

		n_p[g] = positionv;			
		n_v[g] = velocityv;
	}
}