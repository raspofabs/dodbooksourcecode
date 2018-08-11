#include "common.h"
#include <omp.h>

int validated_result;
volatile int output_buffer;

int *a, *b;
int sa, sb;
const int ELEMENT_COUNT = 1 * 1024 * 1024;

void SetupValues() {
	// fill with data
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	sa = sb = 0;
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		sa += a[i] = pcg32_random_r_range(&rng, 0, 255 );
		sb += b[i] = pcg32_random_r_range(&rng, 0, 65535 );
	}
}
void Setup() {
	Timer t;
	// allocate arrays
	a = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	b = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	SetupValues();
	printf( "Setup took %fms\n", t.elapsed() );
}

inline int CalcValue( int i ) {
	return a[i] + b[i];
}

template<int NUM_THREADS>
void TestFalseSharing() {
	int sum=0;
	int aligned_sum_store[NUM_THREADS] __attribute__((aligned(64)));

#pragma omp parallel num_threads(NUM_THREADS)
	{
		int me = omp_get_thread_num();
		aligned_sum_store[me] = 0;

//#pragma omp for
		for (int i = me; i < ELEMENT_COUNT; i += NUM_THREADS ) {
			aligned_sum_store[me] += CalcValue( i );
		}

#pragma omp atomic
		sum += aligned_sum_store[me];
	}
	output_buffer = sum;
}


template<int NUM_THREADS>
void TestLocalAccumulator() {
	int sum=0;

#pragma omp parallel num_threads(NUM_THREADS)
	{
		int me = omp_get_thread_num();
		int local_accumulator = 0;

//#pragma omp for
		for (int i = me; i < ELEMENT_COUNT; i += NUM_THREADS ) {
			local_accumulator += CalcValue( i );
		}

#pragma omp atomic
		sum += local_accumulator;
	}
	output_buffer = sum;
}

template<int NUM_THREADS>
void TestSplitLoad() {
	int sum=0;
	const int WORK_LOAD = ELEMENT_COUNT / NUM_THREADS;

#pragma omp parallel num_threads(NUM_THREADS)
	{
		int me = omp_get_thread_num();
		int local_accumulator = 0;

		const int start = WORK_LOAD * me;
		const int end = WORK_LOAD * (me+1);
//#pragma omp for
		for (int i = start; i < end; ++i ) {
			local_accumulator += CalcValue( i );
		}

#pragma omp atomic
		sum += local_accumulator;
	}
	output_buffer = sum;
}

void TestSinglethreaded() {
	int sum=0;
	// just one thread
	{
		int local_accumulator = 0;

		for (int i = 0; i < ELEMENT_COUNT; i++) {
			local_accumulator += CalcValue( i );
		}

		sum += local_accumulator;
	}
	output_buffer = sum;
}

int main() {
	Setup();

	Test tests[] = {
		(Test){ TestSinglethreaded, "Single threaded" },
		(Test){ TestFalseSharing<2>, "False sharing (2 threads)" },
		(Test){ TestLocalAccumulator<2>, "Local accumulator (2 threads)" },
		(Test){ TestSplitLoad<2>, "Split load (2 threads)" },
		(Test){ TestFalseSharing<4>, "False sharing (4 threads)" },
		(Test){ TestLocalAccumulator<4>, "Local accumulator (4 threads)" },
		(Test){ TestSplitLoad<4>, "Split load (4 threads)" },
		(Test){ TestFalseSharing<8>, "False sharing (8 threads)" },
		(Test){ TestLocalAccumulator<8>, "Local accumulator (8 threads)" },
		(Test){ TestSplitLoad<8>, "Split load (8 threads)" },
		(Test){ TestFalseSharing<16>, "False sharing (16 threads)" },
		(Test){ TestLocalAccumulator<16>, "Local accumulator (16 threads)" },
		(Test){ TestSplitLoad<16>, "Split load (16 threads)" },
	};

	// create a valid result
	TestSinglethreaded();
	validated_result = output_buffer;
	printf( "output_buffer should read %i\n", validated_result );
	TestFalseSharing<4>();
	printf( "Test FalseSharing [%s] output_buffer = %i\n", validated_result==output_buffer ? "PASSED" : "FAILED", output_buffer );
	TestLocalAccumulator<4>();
	printf( "Test LocalAccumul [%s] output_buffer = %i\n", validated_result==output_buffer ? "PASSED" : "FAILED", output_buffer );
	TestSplitLoad<4>();
	printf( "Test SplitLoad    [%s] output_buffer = %i\n", validated_result==output_buffer ? "PASSED" : "FAILED", output_buffer );

	RunTests( tests );

	return validated_result == output_buffer ? 0 : 1;
}
