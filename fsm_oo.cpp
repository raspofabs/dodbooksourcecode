// Finite state machines test
#include "common.h"

const int NUM_MACHINES = 10000;
const int FRAMES_PER_SECOND = 15;
const int NUM_UPDATES = FRAMES_PER_SECOND * 1 * 10; // 10 seconds

const float UPDATE_DELTA = 1.0f / FRAMES_PER_SECOND; // delta in seconds
const float VARIANCE = UPDATE_DELTA * 0.1f;

float HUNGER_TRIGGER = 1.0f;
float SLEEP_HUNGER = 0.1f;
float HUNT_HUNGER = 0.3f;
float EAT_HUNGER = -1.0f;
float EXPLORE_HUNGER = 0.2f;

float SLEEP_TRIGGER = 1.0f;
float SLEEP_SLEEP = -1.0f;
float HUNT_SLEEP = 0.1f;
float EAT_SLEEP = 0.2f;
float EXPLORE_SLEEP = 0.1f;

float HUNTING_TIME = 1.0f;
float EATING_TIME = 0.5f;

#define CHECK_FOR_CONSISTENCY 0
#if CHECK_FOR_CONSISTENCY
const char *REPORT_FORMAT_STRING = "%10s SLEEP %4zi  HUNT %4zi  EAT %4zi  EXPLORE %4zi\n";
const char *PRINT_FORMAT_STRING = "%10s %10s S%+.2f H%+.2f H%+.2f E%+.2f\n";
const char *STATENAME[] = {
	"sleeping",
	"hunting",
	"eating",
	"exploring",
};
#endif
#include "FSM_OOState.h"

struct Data {
	FSMOOState::Data oostate;
};
Data *gData;

void TestFSMOOState() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->oostate.Update( update_time );
	}
};

int main() {
	Data data;
	gData = &data;

	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < 10000; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->oostate.Update( update_time );
	}

	return 0;
}
