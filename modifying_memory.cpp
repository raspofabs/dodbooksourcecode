#include "common.h"

int *c;
//const int BUFFER_SIZE = 4 * 1024 * 1024; // in bytes
const int BUFFER_SIZE = 1024 * 1024; // in bytes

void Setup() {
	Timer t;
	// allocate array
	c = (int*)aligned_alloc( 64, BUFFER_SIZE );
	printf( "Setup took %fms\n", t.elapsed() );
}

template<typename T, size_t NumToParallelModify>
void TestParallelModifyTemplate() {
	T *modify_buffer_ptr = (T*)(void*)c;
	T temp[NumToParallelModify];
	const size_t TOTAL_ELEMENTS = BUFFER_SIZE / sizeof(T);
	for( size_t i = 0; i < TOTAL_ELEMENTS; i+=NumToParallelModify ) {
		for( size_t j = 0; j < NumToParallelModify; ++j ) {
			temp[j] = modify_buffer_ptr[i+j];
			temp[j] += i+j;
			modify_buffer_ptr[i+j] = temp[j];
		}
	}
}
template<typename T, size_t NumToParallelModify>
void TestBatchModifyTemplate() {
	T *modify_buffer_ptr = (T*)(void*)c;
	T temp[NumToParallelModify];
	const size_t TOTAL_ELEMENTS = BUFFER_SIZE / sizeof(T);
	for( size_t i = 0; i < TOTAL_ELEMENTS; i+=NumToParallelModify ) {
		for( size_t j = 0; j < NumToParallelModify; ++j ) {
			temp[j] = modify_buffer_ptr[i+j];
		}
		for( int j = 0; j < NumToParallelModify; ++j ) {
			temp[j] += i+j;
		}
		for( int j = 0; j < NumToParallelModify; ++j ) {
			modify_buffer_ptr[i+j] = temp[j];
		}
	}
}
void TestParallelModify64Bytes() {
	uint8_t *modify_buffer_ptr = (uint8_t*)(void*)c;
	uint8_t temp[64];
	for( int i = 0; i < BUFFER_SIZE; i+=64 ) {
		for( int j = 0; j < 64; ++j ) {
			temp[j] = modify_buffer_ptr[i+j];
			temp[j] += i+j;
			modify_buffer_ptr[i+j] = temp[j];
		}
	}
}

typedef void (*TestFunc)();
struct DetailedTest {
	int typesize, parallelcount;
	TestFunc func;
	char name[64];
	DetailedTest( int ts, int pc, TestFunc f, const char* n ) : typesize(ts), parallelcount(pc), func(f) { strcpy( name, n ); }
};

int main() {
	Setup();

	DetailedTest tests[] = {
		(DetailedTest){ sizeof(int8_t),1, TestParallelModifyTemplate<int8_t,1>, "Modifying 8bit ints, 1 at a time" },
		(DetailedTest){ sizeof(int8_t),2, TestParallelModifyTemplate<int8_t,2>, "Modifying 8bit ints, 2 at a time" },
		(DetailedTest){ sizeof(int8_t),4, TestParallelModifyTemplate<int8_t,4>, "Modifying 8bit ints, 4 at a time" },
		(DetailedTest){ sizeof(int8_t),8, TestParallelModifyTemplate<int8_t,8>, "Modifying 8bit ints, 8 at a time" },
		(DetailedTest){ sizeof(int8_t),16, TestParallelModifyTemplate<int8_t,16>, "Modifying 8bit ints, 16 at a time" },
		(DetailedTest){ sizeof(int8_t),32, TestParallelModifyTemplate<int8_t,32>, "Modifying 8bit ints, 32 at a time" },
		(DetailedTest){ sizeof(int8_t),64, TestParallelModifyTemplate<int8_t,64>, "Modifying 8bit ints, 64 at a time" },

		(DetailedTest){ sizeof(int16_t),1, TestParallelModifyTemplate<int16_t,1>, "Modifying 16bit ints, 1 at a time" },
		(DetailedTest){ sizeof(int16_t),2, TestParallelModifyTemplate<int16_t,2>, "Modifying 16bit ints, 2 at a time" },
		(DetailedTest){ sizeof(int16_t),4, TestParallelModifyTemplate<int16_t,4>, "Modifying 16bit ints, 4 at a time" },
		(DetailedTest){ sizeof(int16_t),8, TestParallelModifyTemplate<int16_t,8>, "Modifying 16bit ints, 8 at a time" },
		(DetailedTest){ sizeof(int16_t),16, TestParallelModifyTemplate<int16_t,16>, "Modifying 16bit ints, 16 at a time" },
		(DetailedTest){ sizeof(int16_t),32, TestParallelModifyTemplate<int16_t,32>, "Modifying 16bit ints, 32 at a time" },
		(DetailedTest){ sizeof(int16_t),64, TestParallelModifyTemplate<int16_t,64>, "Modifying 16bit ints, 64 at a time" },

		(DetailedTest){ sizeof(int32_t),1, TestParallelModifyTemplate<int32_t,1>, "Modifying 32bit ints, 1 at a time" },
		(DetailedTest){ sizeof(int32_t),2, TestParallelModifyTemplate<int32_t,2>, "Modifying 32bit ints, 2 at a time" },
		(DetailedTest){ sizeof(int32_t),4, TestParallelModifyTemplate<int32_t,4>, "Modifying 32bit ints, 4 at a time" },
		(DetailedTest){ sizeof(int32_t),8, TestParallelModifyTemplate<int32_t,8>, "Modifying 32bit ints, 8 at a time" },
		(DetailedTest){ sizeof(int32_t),16, TestParallelModifyTemplate<int32_t,16>, "Modifying 32bit ints, 16 at a time" },
		(DetailedTest){ sizeof(int32_t),32, TestParallelModifyTemplate<int32_t,32>, "Modifying 32bit ints, 32 at a time" },
		(DetailedTest){ sizeof(int32_t),64, TestParallelModifyTemplate<int32_t,64>, "Modifying 32bit ints, 64 at a time" },

		(DetailedTest){ sizeof(int64_t),1, TestParallelModifyTemplate<int64_t,1>, "Modifying 64bit ints, 1 at a time" },
		(DetailedTest){ sizeof(int64_t),2, TestParallelModifyTemplate<int64_t,2>, "Modifying 64bit ints, 2 at a time" },
		(DetailedTest){ sizeof(int64_t),4, TestParallelModifyTemplate<int64_t,4>, "Modifying 64bit ints, 4 at a time" },
		(DetailedTest){ sizeof(int64_t),8, TestParallelModifyTemplate<int64_t,8>, "Modifying 64bit ints, 8 at a time" },
		(DetailedTest){ sizeof(int64_t),16, TestParallelModifyTemplate<int64_t,16>, "Modifying 64bit ints, 16 at a time" },
		(DetailedTest){ sizeof(int64_t),32, TestParallelModifyTemplate<int64_t,32>, "Modifying 64bit ints, 32 at a time" },
		(DetailedTest){ sizeof(int64_t),64, TestParallelModifyTemplate<int64_t,64>, "Modifying 64bit ints, 64 at a time" },

		//(DetailedTest){ sizeof(uint8_t),1, TestBatchModifyTemplate<uint8_t,1>, "Modifying bytes, 1 to a batch" },
		//(DetailedTest){ sizeof(uint8_t),2, TestBatchModifyTemplate<uint8_t,2>, "Modifying bytes, 2 to a batch" },
		//(DetailedTest){ sizeof(uint8_t),4, TestBatchModifyTemplate<uint8_t,4>, "Modifying bytes, 4 to a batch" },
		//(DetailedTest){ sizeof(uint8_t),8, TestBatchModifyTemplate<uint8_t,8>, "Modifying bytes, 8 to a batch" },
		//(DetailedTest){ sizeof(uint8_t),16, TestBatchModifyTemplate<uint8_t,16>, "Modifying bytes, 16 to a batch" },
		//(DetailedTest){ sizeof(uint8_t),32, TestBatchModifyTemplate<uint8_t,32>, "Modifying bytes, 32 to a batch" },
		//(DetailedTest){ sizeof(uint8_t),64, TestBatchModifyTemplate<uint8_t,64>, "Modifying bytes, 64 to a batch" },
		//(DetailedTest){ sizeof(int32_t),1, TestBatchModifyTemplate<int32_t,1>, "Modifying 32bit ints, 1 to a batch" },
		//(DetailedTest){ sizeof(int32_t),2, TestBatchModifyTemplate<int32_t,2>, "Modifying 32bit ints, 2 to a batch" },
		//(DetailedTest){ sizeof(int32_t),4, TestBatchModifyTemplate<int32_t,4>, "Modifying 32bit ints, 4 to a batch" },
		//(DetailedTest){ sizeof(int32_t),8, TestBatchModifyTemplate<int32_t,8>, "Modifying 32bit ints, 8 to a batch" },
		//(DetailedTest){ sizeof(int32_t),16, TestBatchModifyTemplate<int32_t,16>, "Modifying 32bit ints, 16 to a batch" },
		//(DetailedTest){ sizeof(int32_t),32, TestBatchModifyTemplate<int32_t,32>, "Modifying 32bit ints, 32 to a batch" },
		//(DetailedTest){ sizeof(int32_t),64, TestBatchModifyTemplate<int32_t,64>, "Modifying 32bit ints, 64 to a batch" },
		//(DetailedTest){ sizeof(int64_t),1, TestBatchModifyTemplate<int64_t,1>, "Modifying 64bit ints, 1 to a batch" },
		//(DetailedTest){ sizeof(int64_t),2, TestBatchModifyTemplate<int64_t,2>, "Modifying 64bit ints, 2 to a batch" },
		//(DetailedTest){ sizeof(int64_t),4, TestBatchModifyTemplate<int64_t,4>, "Modifying 64bit ints, 4 to a batch" },
		//(DetailedTest){ sizeof(int64_t),8, TestBatchModifyTemplate<int64_t,8>, "Modifying 64bit ints, 8 to a batch" },
		//(DetailedTest){ sizeof(int64_t),16, TestBatchModifyTemplate<int64_t,16>, "Modifying 64bit ints, 16 to a batch" },
		//(DetailedTest){ sizeof(int64_t),32, TestBatchModifyTemplate<int64_t,32>, "Modifying 64bit ints, 32 to a batch" },
		//(DetailedTest){ sizeof(int64_t),64, TestBatchModifyTemplate<int64_t,64>, "Modifying 64bit ints, 64 to a batch" },

		//(DetailedTest){ sizeof(uint8_t),64, TestParallelModify64Bytes, "Modifying bytes, 64 at a time (no template)" },
	};

	RunTests( tests );

	//const int numTests = sizeof( tests ) / sizeof(tests[0]);
	//Timer trialTimer;
	//int trial = 0;
	//while( trial < TRIALS && trialTimer.elapsed() < TRIAL_TIMEOUT ) {
	//	for( auto &test : tests ) {
	//		for( int round = 0; round < ROUNDS; ++round ) {
	//			Timer t;
	//			test.func();
	//			test.timing[round+ROUNDS*trial] = t.elapsed();
	//		}
	//		test.s.Calculate( test.timing, ROUNDS );
	//	}
	//	++trial;
	//}
	//for( auto &test : tests ) {
	//	test.s.Calculate( test.timing, ROUNDS*trial );
	//}

	//printf( "Managed %i trials in %fms\n\n", trial, trialTimer.elapsed() );
	//std::sort( std::begin(tests), std::end(tests),
	//		[]( const Test &a, const Test &b ){
	//		return a.s.average > b.s.average;
	//		} );
	//const char *timesuffix = "ms";
	//if( tests[0].s.average < 2 ) {
	//	timesuffix = "us";
	//	for( auto &test : tests ) {
	//		test.s.Magnify( 1000.0f );
	//	}
	//}

	//
	//for( auto test : tests ) {
	//	printf( "Average% 8.2f%s (95%% +-%.2f%s) for test [%s]\n",
	//	test.s.average, timesuffix,
	//	test.s.standardDeviation*2.0f, timesuffix,
	//	test.name );
	//}
	// write to output file if possible

	//{
	//	std::sort( std::begin(tests), std::end(tests),
	//			[]( const Test &a, const Test &b ){
	//				if( a.typesize != b.typesize )
	//					return a.typesize < b.typesize;
	//				if( a.parallelcount != b.parallelcount )
	//					return a.parallelcount < b.parallelcount;
	//				return false;
	//			} );
	//	char filename[256]; strcpy( filename, __FILE__ ".dat" );
	//	FILE *fp = fopen( filename, "wt" );
	//	if( fp ) {
	//		int sizes[] = { 1,2,4,8 };
	//		for( int s : sizes ) {
	//			for( auto test : tests ) {
	//				if( test.typesize == s ) {
	//					//fprintf( fp, "%i, %f\n", test.parallelcount, test.s.average );
	//					fprintf( fp, "%i, %f, %f\n", test.parallelcount, test.s.average, test.s.standardDeviation );
	//				}
	//			}
	//			fprintf( fp, "\n\n" );
	//		}
	//		fclose( fp );
	//	}
	//}
	return 0;
}
