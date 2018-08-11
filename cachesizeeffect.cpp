#include "common.h"

int *a, *b, *c, *d;
int sa, sb;
const int ELEMENT_COUNT = 2 * 1024 * 1024; // 8Mb of data

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
	c = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	d = (int*)aligned_alloc( 64, sizeof(int) * ELEMENT_COUNT );
	SetupValues();
	printf( "Setup took %fms\n", t.elapsed() );
}

int writeOut;
void UseVariable( int var ) {
	writeOut += var;
}

void TestSummingSimple() {
	int sum = 0;
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		sum += a[i];
	}
	UseVariable( sum );
}
void TestSummingBackwards() {
	int sum = 0;
	for( int i = ELEMENT_COUNT-1; i >= 0; --i ) {
		sum += a[i];
	}
	UseVariable( sum );
}
void TestSummingStrides() {
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
	int mask = (byte_limit / sizeof( c[0] ))-1;
	for( int i = 0; i < ELEMENT_COUNT*16; i+= 16 ) {
		c[i&mask] = i;
	}
}
template<int byte_limit>
void TestModifyRangeLimited() {
	int mask = (byte_limit / sizeof( c[0] ))-1;
	for( int i = 0; i < ELEMENT_COUNT*16; i+= 16 ) {
		c[i&mask] += 1;
	}
}

int main() {
	Setup();

	Test tests[] = {
		(Test){ TestWriteRangeLimited<16*1024>, "Writing within the same 16k" },
		(Test){ TestWriteRangeLimited<32*1024>, "Writing within the same 32k" },
		(Test){ TestWriteRangeLimited<256*1024>, "Writing within the same 256k" },
		(Test){ TestWriteRangeLimited<1*1024*1024>, "Writing within the same 1Mb" },
		(Test){ TestWriteRangeLimited<2*1024*1024>, "Writing within the same 2Mb" },
		(Test){ TestWriteRangeLimited<4*1024*1024>, "Writing within the same 4Mb" },
		(Test){ TestWriteRangeLimited<8*1024*1024>, "Writing within the same 8Mb" },
		(Test){ TestModifyRangeLimited<16*1024>, "Modifying within the same 16k" },
		(Test){ TestModifyRangeLimited<32*1024>, "Modifying within the same 32k" },
		(Test){ TestModifyRangeLimited<256*1024>, "Modifying within the same 256k" },
		(Test){ TestModifyRangeLimited<1*1024*1024>, "Modifying within the same 1Mb" },
		(Test){ TestModifyRangeLimited<2*1024*1024>, "Modifying within the same 2Mb" },
		(Test){ TestModifyRangeLimited<4*1024*1024>, "Modifying within the same 4Mb" },
		(Test){ TestModifyRangeLimited<8*1024*1024>, "Modifying within the same 8Mb" },
	};

	RunTests( tests );

	return 0;
}
