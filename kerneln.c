__kernel void nbody (__global float3* c_p,__global float3* c_v,__global float* m,__global float3* n_p,__global float3* n_v,const float dt, const int s)
{
	int g = get_global_id(0);
	if (g < s)
	{
		float3 positionv= c_p[g];
		float3 velocityv= c_v[g];
		float3 delta;
		float3 accel = (float3) (0.0f,0.0f,0.0f);
		
		float invcube;
		for (int i = 0; i < s; i++)
		{
			delta = c_p[i] - positionv;
			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i];

			accel += invcube * delta;
		}

		positionv += dt * velocityv + (0.5f * dt * dt) * accel;
		velocityv += dt * accel;
		
		n_p[g] = positionv;
		n_v[g] = velocityv;
	}
}