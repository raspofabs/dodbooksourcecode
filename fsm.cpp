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
#include "FSM_Simple.h"
#include "FSM_OOState.h"
#include "FSM_OOImplicitState.h"
#include "FSM_OOFunctional.h"
#include "FSM_TableState.h"
#include "FSM_TableStatePointers.h"
#include "FSM_VaryingTableState.h"

struct Data {
	FSMSimple::Data simple;
	FSMOOState::Data oostate;
	FSMOOImplicitState::Data ooimplicitstate;
	FSMOOFunctional::Data oofunctional;
	FSMTableState::Data tablestate;
	FSMTableStatePointers::Data tablestateptrs;
	FSMVaryingTableState::Data varyingtablestate;
};
Data *gData;

void TestFSMSimple() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->simple.Update( update_time );
	}
};

void TestFSMOOState() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->oostate.Update( update_time );
	}
};

void TestFSMOOImplicitState() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->ooimplicitstate.Update( update_time );
	}
};

void TestFSMOOVirtualFunctional() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->oofunctional.Update( update_time );
	}
};

void TestFSMTableState() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->tablestate.Update( update_time );
	}
};

void TestFSMTableStatePointers() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->tablestateptrs.Update( update_time );
	}
};

void TestFSMVaryingTableState() {
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	for( int update = 0; update < NUM_UPDATES; ++update ) {
		float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
		gData->varyingtablestate.Update( update_time );
	}
};

int main() {
	Data data;
	gData = &data;

#if CHECK_FOR_CONSISTENCY
	{
		pcg32_random_t rng;
		pcg32_srandom_r(&rng, 1234, 5678);
		bool inSync = true;
		for( int update = 0; update < 1000000 && inSync; ++update ) {
			float update_time = pcg32_random_r_rangef(&rng, UPDATE_DELTA - VARIANCE, UPDATE_DELTA + VARIANCE);
			gData->simple.Update( update_time );
			gData->oostate.Update( update_time );
			gData->ooimplicitstate.Update( update_time );
			gData->oofunctional.Update( update_time );
			gData->tablestate.Update( update_time );
			gData->tablestateptrs.Update( update_time );
			gData->varyingtablestate.Update( update_time );

			size_t a[4], b[4];
			gData->simple.Report(a);
			gData->oostate.Report(b);
			if( memcmp( a, b, sizeof( a ) ) ) {
				//printf( REPORT_FORMAT_STRING, "OOSTATE", b[0], b[1], b[2], b[3] );
				//gData->oostate.PrintState();
				inSync = false;
			}
			gData->ooimplicitstate.Report(b);
			if( memcmp( a, b, sizeof( a ) ) ) {
				//printf( REPORT_FORMAT_STRING, "OOIMPLICIT", b[0], b[1], b[2], b[3] );
				//gData->ooimplicitstate.PrintState();
				inSync = false;
			}
			gData->oofunctional.Report(b);
			if( memcmp( a, b, sizeof( a ) ) ) {
				//printf( REPORT_FORMAT_STRING, "OOIMPLICIT", b[0], b[1], b[2], b[3] );
				//gData->oofunctional.PrintState();
				inSync = false;
			}
			gData->tablestate.Report(b);
			if( memcmp( a, b, sizeof( a ) ) ) {
				//printf( REPORT_FORMAT_STRING, "TABLE", b[0], b[1], b[2], b[3] );
				//gData->tablestate.PrintState();
				inSync = false;
			}
			gData->tablestateptrs.Report(b);
			if( memcmp( a, b, sizeof( a ) ) ) {
				//printf( REPORT_FORMAT_STRING, "TABLE", b[0], b[1], b[2], b[3] );
				//gData->tablestate.PrintState();
				inSync = false;
			}
			gData->varyingtablestate.Report(b);
			if( memcmp( a, b, sizeof( a ) ) ) {
				//printf( REPORT_FORMAT_STRING, "VARYING", b[0], b[1], b[2], b[3] );
				//gData->varyingtablestate.PrintState();
				inSync = false;
			}

			if( update > 60000040 ) {
				printf("Update %i:\n", update );
				gData->simple.PrintState();
				gData->oostate.PrintState();
				gData->ooimplicitstate.PrintState();
				gData->oofunctional.PrintState();
				gData->tablestate.PrintState();
				gData->tablestateptrs.PrintState();
				gData->varyingtablestate.PrintState();
			}

			if( !inSync ) {
				printf( REPORT_FORMAT_STRING, "SIMPLE", a[0], a[1], a[2], a[3] );
				gData->simple.PrintState();
				printf( "update %i went out of sync\n", update );
			}
		}
	}
#else
	{

		Test tests[] = {
			(Test){ TestFSMTableState, "Table FSM" },
			(Test){ TestFSMTableStatePointers, "Table FSM (pointers)" },
			(Test){ TestFSMVaryingTableState, "Varying Table FSM" },
			(Test){ TestFSMOOState, "OO FSM" },
			(Test){ TestFSMOOImplicitState, "OO Implicit FSM" },
			(Test){ TestFSMOOVirtualFunctional, "OO Functional FSM" },
			(Test){ TestFSMSimple, "Simple FSM" },
		};

		printf("Trialling with %i machines over %i updates\n", NUM_MACHINES, NUM_UPDATES );
		RunTests( tests );
	}
#endif

	return 0;
}
