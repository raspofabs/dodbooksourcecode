#include "common.h"

int *a, *b, *c; // read from arrays
int sa, sb, sc;
const int ELEMENT_COUNT = 1 * 1024 * 1024;

void SetupValues() {
	// fill with data
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	sa = sb = sc = 0;
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		sa += a[i] = pcg32_random_r_range(&rng, 0, 255 );
		sb += b[i] = pcg32_random_r_range(&rng, 0, 65535 );
		sc += c[i] = pcg32_random_r_range(&rng, 0, 100 );
	}
}
void Setup() {
	Timer t;
	// allocate arrays
	a = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	b = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	c = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	SetupValues();
	printf( "Setup took %fms\n", t.elapsed() );
}

bool SumReadingLinear( int *array, int expected_sum ) {
	int sum = 0;
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		sum += array[i];
	}
	return sum == expected_sum;
}
bool SumReadingBackwards( int *array, int expected_sum ) {
	int sum = 0;
	for( int i = ELEMENT_COUNT-1; i >= 0; --i ) {
		sum += array[i];
	}
	return sum == expected_sum;
}
bool SumReadStriding( int *array, int expected_sum ) {
	int sum = 0;
	const int STRIDE = 16;
	for( int j = 0; j < STRIDE; ++j ) {
		for( int i = 0; i < ELEMENT_COUNT; i+=STRIDE ) {
			sum += array[i];
		}
	}
	return sum == expected_sum;
}

volatile int output_buffer;

void TestReadingBackwards( ) {
	int success = 0;
	success += SumReadingBackwards( a, sa );
	success += SumReadingBackwards( b, sb );
	success += SumReadingBackwards( c, sc );
	output_buffer += success;
}
void TestReadingLinear() {
	int success = 0;
	success += SumReadingLinear( a, sa );
	success += SumReadingLinear( b, sb );
	success += SumReadingLinear( c, sc );
	output_buffer += success;
}
void TestReadStriding() {
	int success = 0;
	success += SumReadStriding( a, sa );
	success += SumReadStriding( b, sb );
	success += SumReadStriding( c, sc );
	output_buffer += success;
}

int main() {
	Setup();

	Test tests[] = {
		(Test){ TestReadingBackwards, "Read Backwards" },
		(Test){ TestReadingLinear, "Read Linear" },
		(Test){ TestReadStriding, "Read Striding" },
	};

	RunTests( tests );
	return 0;
}
