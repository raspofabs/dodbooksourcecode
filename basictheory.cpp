#include "common.h"

const int NUM_TESTS = 11;
int *A[NUM_TESTS], *B[NUM_TESTS], *C[NUM_TESTS], *D[NUM_TESTS];
int *a, *b, *c, *d;
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
void PrepTest() {
	static int lastTest = NUM_TESTS-1;
	static int test = 0;
	A[lastTest] = a; B[lastTest] = b; C[lastTest] = c; D[lastTest] = d;
	a = A[test]; b = B[test]; c = C[test]; d = D[test];
	lastTest = test;
	test = (test+1) % NUM_TESTS;
}
void Setup() {
	Timer t;
	// allocate arrays
	for( int test = 0; test < NUM_TESTS; ++test ) {
		PrepTest();
		a = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
		b = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
		c = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
		d = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
		SetupValues();
	}
	printf( "Setup took %fms\n", t.elapsed() );
}

int writeOut;
void UseVariable( int var ) {
	writeOut += var;
}

void TestSummingSimple() {
	PrepTest();
	int sum = 0;
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		sum += a[i];
	}
	UseVariable( sum );
}
void TestSummingBackwards() {
	PrepTest();
	int sum = 0;
	for( int i = ELEMENT_COUNT-1; i >= 0; --i ) {
		sum += b[i];
	}
	UseVariable( sum );
}
void TestSummingStrides() {
	PrepTest();
	int sum = 0;
	const int STRIDE = 16;
	for( int offset = 0; offset < STRIDE; offset += 1 ) {
		for( int i = offset; i < ELEMENT_COUNT; i += STRIDE ) {
			sum += a[i];
		}
	}
	UseVariable( sum );
}
template<int byte_limit>
void TestWriteRangeLimited() {
	PrepTest();
	int mask = (byte_limit / sizeof( c[0] ))-1;
	for( int i = 0; i != ELEMENT_COUNT*16; i+= 16 ) {
		c[i&mask] = i;
	}
}
void TestWriteSimple() {
	PrepTest();
	for( int i = 0; i != ELEMENT_COUNT; ++i ) {
		c[i] = i;
	}
}
void TestWriteBackwards() {
	PrepTest();
	for( int i = ELEMENT_COUNT-1; i >= 0; --i ) {
		c[i] = i;
	}
}
void TestWriteStrides() {
	PrepTest();
	const int STRIDE = 16;
	for( int offset = 0; offset < STRIDE; offset += 1 ) {
		for( int i = offset; i < ELEMENT_COUNT; i += STRIDE ) {
			c[i] = i;
		}
	}
}
void TestSimpleCopy() {
	PrepTest();
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		c[i] = a[i];
	}
}
void TestMultiRead() {
	PrepTest();
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		c[i] = a[i] + b[i];
	}
}
void TestMultiWrite() {
	PrepTest();
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		c[i] = a[i];
		d[i] = a[i];
	}
}
void TestMultiBoth() {
	PrepTest();
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		c[i] = a[i] + b[i];
		d[i] = a[i] - b[i];
	}
}

void TestWriteAndModifyPaired() {
	PrepTest();
	for( int i = 0; i < ELEMENT_COUNT; i+=2 ) {
		c[i] = c[i] + b[i];
		c[i+1] = b[i];
	}
}
void TestWriteAndModifySeparate() {
	PrepTest();
	const int HALF_ELEMENT_COUNT = ELEMENT_COUNT / 2;
	for( int i = 0; i < HALF_ELEMENT_COUNT; ++i ) {
		c[i] = c[i] + b[i];
		c[i+HALF_ELEMENT_COUNT] = b[i];
	}
}

int main() {
	Setup();

	{
		Test tests[] = {
			(Test){ TestSimpleCopy, "Simple Copy" },
			(Test){ TestMultiRead, "Summing Copy" },
			(Test){ TestMultiWrite, "Splitting Copy" },
			(Test){ TestMultiBoth, "Splitting Combine Copy" },

			(Test){ TestWriteSimple, "Write Forwards" },
			(Test){ TestWriteBackwards, "Write Backwards" },
			(Test){ TestWriteStrides, "Write in Strides" },

			(Test){ TestSummingSimple, "Summing Forwards" },
			(Test){ TestSummingBackwards, "Summing Backwards" },
			(Test){ TestSummingStrides, "Summing in Strides" },

			(Test){ TestWriteAndModifyPaired, "Write and modify" },
			(Test){ TestWriteAndModifySeparate, "Write only separated" },
		};

		RunTests( tests );
	}

	return 0;
}
