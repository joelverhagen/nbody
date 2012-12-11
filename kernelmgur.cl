__kernel void nbody (__global float3* l_c_p,__global float3* l_c_v,__global float3* e_c_p,__global float3* e_c_v,__global float* m,__global float3* n_p,__global float3* n_v,const float dt, const int s, const int e, const int gid)
{
	int g = get_global_id(0);
	if (g < s)
	{
		float3 posiionv= l_c_p[g];
		float3 velocityv= l_c_v[g];
		float3 delta;
		float3 accel = (float3) (0.0f,0.0f,0.0f);		
		float invcube;

		int i = 0;
		while (i < s)
		{
			delta = l_c_p[i] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i + e * gid];
			accel += invcube * delta;

			delta = l_c_p[i+1] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i +1 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[i+2] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+2 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[i+3] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+3 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[i+4] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+4 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[i+5] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i +5 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[i+6] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+6 + e * gid];
			accel += invcube * delta;

			delta = l_c_p[i+7] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+7 + e * gid];
			accel += invcube * delta;
		
			i+=8;
		}

		i = 0;
		while (i < e)
		{
			delta = e_c_p[i] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i + s * (1 - gid)];
			accel += invcube * delta;

			delta = e_c_p[i+1] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+1 + s * (1 - gid)];
			accel += invcube * delta;

			delta = e_c_p[i+2] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+2 + s * (1 - gid)];
			accel += invcube * delta;

			delta = e_c_p[i+3] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+3 + s * (1 - gid)];
			accel += invcube * delta;

			delta = l_c_p[i+4] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+4 + s * (1 - gid)];
			accel += invcube * delta;

			delta = l_c_p[i+5] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i +5 + s * (1 - gid)];
			accel += invcube * delta;

			delta = l_c_p[i+6] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+6 + s * (1 - gid)];
			accel += invcube * delta;

			delta = l_c_p[i+7] - posiionv;			
			invcube = rsqrt(dot(delta,delta)+ 500.0f);
			invcube *= invcube *invcube;
			invcube *= m[i+7 + s * (1 - gid)];
			accel += invcube * delta;

			i+= 8;
		}

		posiionv += dt * velocityv + (0.5f * dt * dt) * accel;
		velocityv += dt * accel;


		n_p[g] = posiionv;
		n_v[g] = velocityv;
	}
}