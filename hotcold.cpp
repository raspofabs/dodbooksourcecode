// Hot Cold data separation
#include "common.h"

const int NUM_PARTICLES = 10000;
const int FRAMES_PER_SECOND = 60;
const int NUM_UPDATES = FRAMES_PER_SECOND * 10; // ten seconds of particle updates at 60fps;

const float UPDATE_DELTA = 1000.0f / FRAMES_PER_SECOND; // delta in ms

struct particle_buffer_Simple {
	struct particle {
		Vec3 pos;
		Vec3 velocity;
		float lifetime;
		uint32_t colour;
		float size;
		uint32_t materialOrUVLookupData;
	};
	particle *p;
	particle_buffer_Simple() {
		p = (particle*)malloc( sizeof(particle) * NUM_PARTICLES );

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			p[i].pos.x = (i%7)-3;
			p[i].pos.y = (i%11)-5;
			p[i].pos.z = (i%9)-4;

			p[i].velocity.x = 2.0f;
			p[i].velocity.y = 100.0f;
			p[i].velocity.z = 7.0f;

			p[i].lifetime = 10.0f;
			p[i].colour = 0xffeebbff;
			p[i].size = 0.5f;
			p[i].materialOrUVLookupData = 12415;
		}
	}
	~particle_buffer_Simple() {
		free(p);
	}
};
struct particle_buffer_HotColdSplit {
	struct particle_hot {
		Vec3 pos;
		Vec3 velocity;
	};
	struct particle_cold {
		float lifetime;
		uint32_t colour;
		float size;
		uint32_t materialOrUVLookupData;
	};
	particle_hot *ph;
	particle_cold *pc;
	particle_buffer_HotColdSplit() {
		ph = (particle_hot*)malloc( sizeof(particle_hot) * NUM_PARTICLES );
		pc = (particle_cold*)malloc( sizeof(particle_cold) * NUM_PARTICLES );

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			ph[i].pos.x = (i%7)-3;
			ph[i].pos.y = (i%11)-5;
			ph[i].pos.z = (i%9)-4;

			ph[i].velocity.x = 2.0f;
			ph[i].velocity.y = 100.0f;
			ph[i].velocity.z = 7.0f;

			pc[i].lifetime = 10.0f;
			pc[i].colour = 0xffeebbff;
			pc[i].size = 0.5f;
			pc[i].materialOrUVLookupData = 12415;
		}
	}
	~particle_buffer_HotColdSplit() {
		free(ph);
		free(pc);
	}
};
struct particle_buffer_ReadWriteSplit {
	struct particle_read {
		Vec3 velocity;
	};
	struct particle_write {
		Vec3 pos;
	};
	struct particle_cold {
		float lifetime;
		uint32_t colour;
		float size;
		uint32_t materialOrUVLookupData;
	};
	particle_read *pr;
	particle_write *pw;
	particle_cold *pc;
	particle_buffer_ReadWriteSplit() {
		pr = (particle_read*)malloc( sizeof(particle_read) * NUM_PARTICLES );
		pw = (particle_write*)malloc( sizeof(particle_write) * NUM_PARTICLES );
		pc = (particle_cold*)malloc( sizeof(particle_cold) * NUM_PARTICLES );

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			pw[i].pos.x = (i%7)-3;
			pw[i].pos.y = (i%11)-5;
			pw[i].pos.z = (i%9)-4;

			pr[i].velocity.x = 2.0f;
			pr[i].velocity.y = 100.0f;
			pr[i].velocity.z = 7.0f;

			pc[i].lifetime = 10.0f;
			pc[i].colour = 0xffeebbff;
			pc[i].size = 0.5f;
			pc[i].materialOrUVLookupData = 12415;
		}
	}
	~particle_buffer_ReadWriteSplit() {
		free(pr);
		free(pw);
		free(pc);
	}
};

struct Data {
	particle_buffer_Simple pbSimple;
	particle_buffer_HotColdSplit pbHotCold;
	particle_buffer_ReadWriteSplit pbReadWrite;
};
Data *gData;


void TestUpdateParticles_Simple() {
	particle_buffer_Simple *pb = &gData->pbSimple;
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int u = 0; u < NUM_UPDATES; ++u ) {
		// ensure that compiler cannot optimise out delta_time, but it remains repeatable
		float delta_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA * 0.9f, UPDATE_DELTA * 1.1f );

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			particle_buffer_Simple::particle *p = pb->p+i;
			p->pos += p->velocity * delta_time;
		}
	}
}
void TestUpdateParticles_HotColdSplit() {
	particle_buffer_HotColdSplit *pb = &gData->pbHotCold;
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int u = 0; u < NUM_UPDATES; ++u ) {
		// ensure that compiler cannot optimise out delta_time, but it remains repeatable
		float delta_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA * 0.9f, UPDATE_DELTA * 1.1f );

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			particle_buffer_HotColdSplit::particle_hot *p = pb->ph+i;
			p->pos += p->velocity * delta_time;
		}
	}
}
void TestUpdateParticles_ReadWriteSplit() {
	particle_buffer_ReadWriteSplit *pb = &gData->pbReadWrite;
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int u = 0; u < NUM_UPDATES; ++u ) {
		// ensure that compiler cannot optimise out delta_time, but it remains repeatable
		float delta_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA * 0.9f, UPDATE_DELTA * 1.1f );

		for( int i = 0; i < NUM_PARTICLES; ++i ) {
			particle_buffer_ReadWriteSplit::particle_read *pr = pb->pr+i;
			particle_buffer_ReadWriteSplit::particle_write *pw = pb->pw+i;
			pw->pos += pr->velocity * delta_time;
		}
	}
}


int main() {
	Data data;
	gData = &data;

	Test tests[] = {
		(Test){ TestUpdateParticles_Simple, "Simple struct array" },
		(Test){ TestUpdateParticles_HotColdSplit, "HotCold split struct array" },
		(Test){ TestUpdateParticles_ReadWriteSplit, "ReadWrite split struct array" },
	};

	printf("Trialling with %i particles over %i updates\n", NUM_PARTICLES, NUM_UPDATES );
	RunTests( tests );

	return 0;
}
