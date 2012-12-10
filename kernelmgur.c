__kernel void nbody (__global float3* l_c_p,__global float3* l_c_v,__global float3* e_c_p,__global float3* e_c_v,__global float* m,__global float3* n_p,__global float3* n_v,const float dt, const int s, const int e, const int gid)
{
	int g = get_global_id(0);
	if (g < s)
	{
		float3 positionv= l_c_p[g];
		float3 velocityv= l_c_v[g];
		float3 delta;
		float3 accel = (float3) (0.0f,0.0f,0.0f);
		
		float invcube;
		int su = s / 8;
		int ti;
		for (int i = 0; i < su; i++)
		{
			ti = i*8;
			delta = l_c_p[ti] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+1] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti +1 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+2] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+2 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+3] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+3 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+4] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+4 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+5] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti +5 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+6] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+6 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+7] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+7 + e * gid];
			accel += invcube * delta;
		}

		int eu = e / 8;
		for (int i = 0; i < eu; i++)
		{
			ti = i*8;
			delta = e_c_p[ti] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti + s * (1 - gid)];
			accel += invcube * delta;

			delta = e_c_p[ti+1] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+1 + s * (1 - gid)];
			accel += invcube * delta;

			delta = e_c_p[ti+2] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+2 + s * (1 - gid)];
			accel += invcube * delta;

			delta = e_c_p[ti+3] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+3 + s * (1 - gid)];
			accel += invcube * delta;

			delta = l_c_p[ti+4] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+4 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+5] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti +5 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+6] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+6 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[ti+7] - positionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[ti+7 + e * gid];
			accel += invcube * delta;
		}

		positionv += dt * velocityv + (0.5f * dt * dt) * accel;
		velocityv += dt * accel;


		n_p[g] = positionv;
		n_v[g] = velocityv;
	}
}