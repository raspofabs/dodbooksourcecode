#include "common.h"

int validated_result;
volatile int output_buffer;

int *a1, *b1;
int *a2, *b2;
const int ELEMENT_COUNT = 1 * 1024 * 1024;

void SetupValues() {
	// fill with data
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	std::vector<int> sortingarray;
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		a1[i] = pcg32_random_r_range(&rng, 0, 255 );
		b1[i] = pcg32_random_r_range(&rng, 0, 65535 );
		sortingarray.push_back( i );
	}
	std::sort( sortingarray.begin(), sortingarray.end(), 
			[]( int first, int second ){
				return a1[first] < a1[second];
			} );
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		a2[i] = a1[sortingarray[i]];
		b2[i] = b1[sortingarray[i]];
	}
}
void Setup() {
	Timer t;
	// allocate arrays
	a1 = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	b1 = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	a2 = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	b2 = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	SetupValues();
	printf( "Setup took %fms\n", t.elapsed() );
}

int lookup[256];

void ClearCalcs() {
	for( int i = 0; i < 256; ++i ) {
		lookup[i] = 3000 - i*3;
	}
}
int CalculateForHigh( int a, int b ) {
	return (a^b) + lookup[a&255] + b;
}
int CalculateForLow( int a, int b ) {
	return lookup[b&255] + ( (a*b) > 100 ? 7 : 3 );
}


void TrivialRandomBranching() {
	int sum=0;
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if( a1[i] > 128 ) {
			sum += b1[i];
		}
	}
	output_buffer = sum;
}

void TrivialSortedBranching() {
	int sum=0;
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if( a2[i] > 128 ) {
			sum += b2[i];
		}
	}
	output_buffer = sum;
}
void RealisticRandomBranching() {
	int sum=0;
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if( a1[i] > 128 ) {
			sum += CalculateForHigh( a1[i], b1[i] );
		} else {
			sum += CalculateForLow( a1[i], b1[i] );
		}
	}
	output_buffer = sum;
}

void RealisticSortedBranching() {
	int sum=0;
	for (int i = 0; i < ELEMENT_COUNT; i++) {
		if( a2[i] > 128 ) {
			sum += CalculateForHigh( a2[i], b2[i] );
		} else {
			sum += CalculateForLow( a2[i], b2[i] );
		}
	}
	output_buffer = sum;
}

int main() {
	ClearCalcs();
	Setup();

	Test tests[] = {
		(Test){ TrivialRandomBranching, "Trivial Random branching" },
		(Test){ TrivialSortedBranching, "Trivial Sorted branching" },
		(Test){ RealisticRandomBranching, "Random branching" },
		(Test){ RealisticSortedBranching, "Sorted branching" },
	};

	// create a valid result
	TrivialRandomBranching();
	validated_result = output_buffer;
	printf( "output_buffer should read %i\n", validated_result );
	TrivialSortedBranching();
	printf( "Test TrivialSortedBranching [%s] output_buffer = %i\n", validated_result==output_buffer ? "PASSED" : "FAILED", output_buffer );
	RealisticRandomBranching();
	validated_result = output_buffer;
	printf( "Test RealisticRandomBranching [%s] output_buffer = %i\n", validated_result==output_buffer ? "PASSED" : "FAILED", output_buffer );
	RealisticSortedBranching();
	printf( "Test RealisticSortedBranching [%s] output_buffer = %i\n", validated_result==output_buffer ? "PASSED" : "FAILED", output_buffer );

	RunTests( tests );

	return 0;
}
