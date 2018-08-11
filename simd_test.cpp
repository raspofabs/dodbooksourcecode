// SIMD particle update
#include "common.h"
#if __SSE__
#include <emmintrin.h>
#endif
#if __AVX__
#include <immintrin.h>
#endif

const int NUM_PARTICLES = 10000;
const int FRAMES_PER_SECOND = 60;
const int NUM_UPDATES = FRAMES_PER_SECOND * 10; // ten seconds of particle updates at 60fps;

const float UPDATE_DELTA = 1000.0f / FRAMES_PER_SECOND; // delta in ms

struct particle_buffer_AoS {
	struct particle {
		float x,y,z,vx,vy,vz,t;
	};
	particle *p;
	float gravity;
	particle_buffer_AoS() {
		p = (particle*)malloc( sizeof(particle) * NUM_PARTICLES );
		gravity = -9.81f;

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			p[i].x = (i%7)-3;
			p[i].y = (i%11)-5;
			p[i].z = (i%9)-4;

			p[i].vx = 2.0f;
			p[i].vy = 100.0f;
			p[i].vz = 7.0f;

			p[i].t = 0.0f;
		}
	}
	~particle_buffer_AoS() {
		free(p);
	}
};

struct particle_buffer {
	float *posx, *posy, *posz;
	float *vx, *vy, *vz;
	float gravity;
	particle_buffer() {
		posx = (float*)aligned_alloc( 32, sizeof(float) * NUM_PARTICLES );
		posy = (float*)aligned_alloc( 32, sizeof(float) * NUM_PARTICLES );
		posz = (float*)aligned_alloc( 32, sizeof(float) * NUM_PARTICLES );
		vx = (float*)aligned_alloc( 32, sizeof(float) * NUM_PARTICLES );
		vy = (float*)aligned_alloc( 32, sizeof(float) * NUM_PARTICLES );
		vz = (float*)aligned_alloc( 32, sizeof(float) * NUM_PARTICLES );

		gravity = -9.81f;

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			posx[i] = (i%7)-3;
			posy[i] = (i%11)-5;
			posz[i] = (i%9)-4;

			vx[i] = 2.0f;
			vy[i] = 100.0f;
			vz[i] = 7.0f;
		}
	}
	~particle_buffer() {
		free(posx);
		free(posy);
		free(posz);
		free(vx);
		free(vy);
		free(vz);
	}
};

void SimpleUpdateParticlesAoS( particle_buffer_AoS *pb, float delta_time ) {
	float g = pb->gravity;
	float gd2 = g * delta_time * delta_time * 0.5f;
	float gd = g * delta_time;
	for( int i = 0; i < NUM_PARTICLES; ++i ) {
		particle_buffer_AoS::particle *p = pb->p+i;
		p->x += p->vx * delta_time;
		p->y += p->vy * delta_time + gd2;
		p->z += p->vz * delta_time;
		p->vy += gd;
	}
}

void SimpleUpdateParticles( particle_buffer *pb, float delta_time ) {
	float g = pb->gravity;
	float gd2 = g * delta_time * delta_time * 0.5f;
	float gd = g * delta_time;
	for( int i = 0; i < NUM_PARTICLES; ++i ) {
		pb->posx[i] += pb->vx[i] * delta_time;
		pb->posy[i] += pb->vy[i] * delta_time + gd2;
		pb->posz[i] += pb->vz[i] * delta_time;
		pb->vy[i] += gd;
	}
}
void SliceUpdateParticles( particle_buffer *pb, float delta_time ) {
	float g = pb->gravity;
	float gd2 = g * delta_time * delta_time * 0.5f;
	float gd = g * delta_time;
	for( int i = 0; i < NUM_PARTICLES; ++i ) {
		pb->posx[i] += pb->vx[i] * delta_time;
	}
	for( int i = 0; i < NUM_PARTICLES; ++i ) {
		pb->posy[i] += pb->vy[i] * delta_time + gd2;
		pb->vy[i] += gd;
	}
	for( int i = 0; i < NUM_PARTICLES; ++i ) {
		pb->posz[i] += pb->vz[i] * delta_time;
	}
}
#if __SSE__
void SIMD_SSE_UpdateParticles( particle_buffer *pb, float delta_time ) {
	float g = pb->gravity;
	float f_gd = g * delta_time;
	float f_gd2 = pb->gravity * delta_time * delta_time * 0.5f;

	// delta_time
	__m128 mmd = _mm_setr_ps( delta_time, delta_time, delta_time, delta_time );
	// gravity * delta_time
	__m128 mmgd = _mm_load1_ps( &f_gd );
	// gravity * delta_time * delta_time * 0.5f
	__m128 mmgd2 = _mm_load1_ps( &f_gd2 );

	__m128 *px = (__m128*)pb->posx;
	__m128 *py = (__m128*)pb->posx;
	__m128 *pz = (__m128*)pb->posz;
	__m128 *vx = (__m128*)pb->vx;
	__m128 *vy = (__m128*)pb->vy;
	__m128 *vz = (__m128*)pb->vz;

	int iterationCount = NUM_PARTICLES / 4;
	for( int i = 0; i < iterationCount; ++i ) {
		__m128 dx = _mm_mul_ps(vx[i], mmd );
		__m128 dy = _mm_add_ps( _mm_mul_ps(vy[i], mmd ), mmgd2 );
		__m128 dz = _mm_mul_ps(vz[i], mmd );
		__m128 newx = _mm_add_ps(px[i], dx);
		__m128 newy = _mm_add_ps(py[i], dy);
		__m128 newz = _mm_add_ps(pz[i], dz);
		__m128 newvy = _mm_add_ps(vy[i], mmgd);
		_mm_store_ps((float*)(px+i), newx);
		_mm_store_ps((float*)(py+i), newy);
		_mm_store_ps((float*)(pz+i), newz);
		_mm_store_ps((float*)(vy+i), newvy);
	}
}
void SIMD_SSE_UpdateParticlesSliced( particle_buffer *pb, float delta_time ) {
	float g = pb->gravity;
	float f_gd = g * delta_time;
	float f_gd2 = pb->gravity * delta_time * delta_time * 0.5f;

	// delta_time
	__m128 mmd = _mm_setr_ps( delta_time, delta_time, delta_time, delta_time );
	// gravity * delta_time
	__m128 mmgd = _mm_load1_ps( &f_gd );
	// gravity * delta_time * delta_time * 0.5f
	__m128 mmgd2 = _mm_load1_ps( &f_gd2 );

	__m128 *px = (__m128*)pb->posx;
	__m128 *py = (__m128*)pb->posx;
	__m128 *pz = (__m128*)pb->posz;
	__m128 *vx = (__m128*)pb->vx;
	__m128 *vy = (__m128*)pb->vy;
	__m128 *vz = (__m128*)pb->vz;

	int iterationCount = NUM_PARTICLES / 4;
	for( int i = 0; i < iterationCount; ++i ) {
		__m128 dx = _mm_mul_ps(vx[i], mmd );
		__m128 newx = _mm_add_ps(px[i], dx);
		_mm_store_ps((float*)(px+i), newx);
	}
	for( int i = 0; i < iterationCount; ++i ) {
		__m128 dy = _mm_add_ps( _mm_mul_ps(vy[i], mmd ), mmgd2 );
		__m128 newy = _mm_add_ps(py[i], dy);
		__m128 newvy = _mm_add_ps(vy[i], mmgd);
		_mm_store_ps((float*)(py+i), newy);
		_mm_store_ps((float*)(vy+i), newvy);
	}
	for( int i = 0; i < iterationCount; ++i ) {
		__m128 dz = _mm_mul_ps(vz[i], mmd );
		__m128 newz = _mm_add_ps(pz[i], dz);
		_mm_store_ps((float*)(pz+i), newz);
	}
}
#endif

#if __AVX__
void SIMD_AVX_UpdateParticles( particle_buffer *pb, float delta_time ) {
	float g = pb->gravity;
	float f_gd = g * delta_time;
	float f_gd2 = pb->gravity * delta_time * delta_time * 0.5f;

	// delta_time
	__m256 mm256d = _mm256_set1_ps( delta_time );
	// gravity * delta_time
	__m256 mm256gd = _mm256_set1_ps( f_gd );
	// gravity * delta_time * delta_time * 0.5f
	__m256 mm256gd2 = _mm256_set1_ps( f_gd2 );

	__m256 *px = (__m256*)pb->posx;
	__m256 *py = (__m256*)pb->posx;
	__m256 *pz = (__m256*)pb->posz;
	__m256 *vx = (__m256*)pb->vx;
	__m256 *vy = (__m256*)pb->vy;
	__m256 *vz = (__m256*)pb->vz;

	int iterationCount = NUM_PARTICLES / 8;
	for( int i = 0; i < iterationCount; ++i ) {
		__m256 dx = _mm256_mul_ps(vx[i], mm256d );
		__m256 dy = _mm256_add_ps( _mm256_mul_ps(vy[i], mm256d ), mm256gd2 );
		__m256 dz = _mm256_mul_ps(vz[i], mm256d );
		__m256 newx = _mm256_add_ps(px[i], dx);
		__m256 newy = _mm256_add_ps(py[i], dy);
		__m256 newz = _mm256_add_ps(pz[i], dz);
		__m256 newvy = _mm256_add_ps(vy[i], mm256gd);
		_mm256_store_ps((float*)(px+i), newx);
		_mm256_store_ps((float*)(py+i), newy);
		_mm256_store_ps((float*)(pz+i), newz);
		_mm256_store_ps((float*)(vy+i), newvy);
	}
}
#endif

struct Data {
	particle_buffer_AoS pbAoS;
	particle_buffer pbsimple;
	particle_buffer pbslice;
	particle_buffer pbSIMDSSE;
	particle_buffer pbSIMDAVX;
};
Data *gData;

#if __AVX__
void TestAVX() {
	for( int frame = 0; frame < NUM_UPDATES; ++frame ) {
		SIMD_AVX_UpdateParticles( &gData->pbSIMDAVX, UPDATE_DELTA);
	}
}
#endif
#if __SSE__
void TestSSE() {
	for( int frame = 0; frame < NUM_UPDATES; ++frame ) {
		SIMD_SSE_UpdateParticles( &gData->pbSIMDSSE, UPDATE_DELTA);
	}
}
void TestSSESliced() {
	for( int frame = 0; frame < NUM_UPDATES; ++frame ) {
		SIMD_SSE_UpdateParticlesSliced( &gData->pbSIMDSSE, UPDATE_DELTA);
	}
}
#endif
void TestSoASliced() {
	for( int frame = 0; frame < NUM_UPDATES; ++frame ) {
		SliceUpdateParticles( &gData->pbslice, UPDATE_DELTA);
	}
}
void TestSoA() {
	for( int frame = 0; frame < NUM_UPDATES; ++frame ) {
		SimpleUpdateParticles( &gData->pbsimple, UPDATE_DELTA);
	}
}
void TestAoS() {
	for( int frame = 0; frame < NUM_UPDATES; ++frame ) {
		SimpleUpdateParticlesAoS( &gData->pbAoS, UPDATE_DELTA);
	}
}

int main() {
	Data data;
	gData = &data;

	Test tests[] = {
		(Test){ TestAoS, "array of structs" },
		(Test){ TestSoA, "struct of arrays naive processing" },
		(Test){ TestSoASliced, "struct of arrays partitioned processing" },
#if __AVX__
		(Test){ TestAVX, "AVX" },
#endif
#if __SSE__
		(Test){ TestSSE, "SSE" },
		(Test){ TestSSESliced, "SSE partitioned" },
#endif
	};


	printf("Trialling with %i particles over %i updates\n", NUM_PARTICLES, NUM_UPDATES );
	RunTests( tests );
	return 0;
}
