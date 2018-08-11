// Hot Cold data separation
#include "common.h"

const int MATRIX_SIZE = 1024;

struct LargeMatrix {
	float m[MATRIX_SIZE * MATRIX_SIZE];
};

struct Data {
	LargeMatrix from, to;
};
Data *gData;


void TestTranspose_ReadRows() {
	float *in = &(gData->from.m[0]);
	float *out = &(gData->to.m[0]);

	for( int j = 0; j < MATRIX_SIZE; j++ ) {
		for( int i = 0; i < MATRIX_SIZE; i++ ) {
			out[i*MATRIX_SIZE+j]=in[j*MATRIX_SIZE+i];
		}
	}
}
void TestTranspose_ReadColumns() {
	float *in = &(gData->from.m[0]);
	float *out = &(gData->to.m[0]);

	for( int i = 0; i < MATRIX_SIZE; i++ ) {
		for( int j = 0; j < MATRIX_SIZE; j++ ) {
			out[i*MATRIX_SIZE+j]=in[j*MATRIX_SIZE+i];
		}
	}
}
template<int block_size>
void TestTranspose_RowBlock() {
	float *in = &(gData->from.m[0]);
	float *out = &(gData->to.m[0]);

	for (int bj = 0; bj < MATRIX_SIZE; bj += block_size) {
		for (int bi = 0; bi < MATRIX_SIZE; bi += block_size) {
			int imax = bi + block_size; imax = imax < MATRIX_SIZE ? imax : MATRIX_SIZE;
			int jmax = bj + block_size; jmax = jmax < MATRIX_SIZE ? jmax : MATRIX_SIZE;
			for (int j = bj; j < jmax; ++j) {
				for (int i = bi; i < imax; ++i) {
					out[i*MATRIX_SIZE+j] = in[j*MATRIX_SIZE+i];
				}
			}
		}
	}
}
template<int block_size>
void TestTranspose_ColumnBlock() {
	float *in = &(gData->from.m[0]);
	float *out = &(gData->to.m[0]);

	for (int bi = 0; bi < MATRIX_SIZE; bi += block_size) {
		for (int bj = 0; bj < MATRIX_SIZE; bj += block_size) {
			int imax = bi + block_size;
			int jmax = bj + block_size;
			// these cause an overflow assumption warning on newer gcc compilers (found on 6.3.0)
			//imax = imax < MATRIX_SIZE ? imax : MATRIX_SIZE;
			//jmax = jmax < MATRIX_SIZE ? jmax : MATRIX_SIZE;
			for (int i = bi; i < imax; ++i) {
				for (int j = bj; j < jmax; ++j) {
					out[i*MATRIX_SIZE+j] = in[j*MATRIX_SIZE+i];
				}
			}
		}
	}
}
template<int block_size>
void TestTranspose_WriteBlock() {
	float *in = &(gData->from.m[0]);
	float *out = &(gData->to.m[0]);

	for (int bj = 0; bj < MATRIX_SIZE; bj += block_size) {
		int jmax = bj + block_size; jmax = jmax < MATRIX_SIZE ? jmax : MATRIX_SIZE;
		for (int i = 0; i < MATRIX_SIZE; ++i) {
			for (int j = bj; j < jmax; ++j) {
				out[i*MATRIX_SIZE+j] = in[j*MATRIX_SIZE+i];
			}
		}
	}
}
template<int read_block, int write_block>
void TestTranspose_RowBlock2() {
	float *in = &(gData->from.m[0]);
	float *out = &(gData->to.m[0]);

	for (int bi = 0; bi < MATRIX_SIZE; bi += read_block) {
		for (int bj = 0; bj < MATRIX_SIZE; bj += write_block) {
			int imax = bi + read_block; imax = imax < MATRIX_SIZE ? imax : MATRIX_SIZE;
			int jmax = bj + write_block; jmax = jmax < MATRIX_SIZE ? jmax : MATRIX_SIZE;
			for (int i = bi; i < imax; ++i) {
				for (int j = bj; j < jmax; ++j) {
					out[i*MATRIX_SIZE+j] = in[j*MATRIX_SIZE+i];
				}
			}
		}
	}
}

int main() {
	char *dataAlloc = (char*)aligned_alloc( 64, sizeof( Data ) + 64 );

	gData = (Data*)(void*)(dataAlloc);

	Test tests[] = {
		(Test){ TestTranspose_ReadRows, "Read rows, write columns" },
		(Test){ TestTranspose_ReadColumns, "Read columns, write rows" },

		(Test){ TestTranspose_RowBlock<4>, "Read rows, blocks of 4" },
		(Test){ TestTranspose_RowBlock<8>, "Read rows, blocks of 8" },
		(Test){ TestTranspose_RowBlock<16>, "Read rows, blocks of 16" },

		(Test){ TestTranspose_ColumnBlock<4>, "Read columns, blocks of 4" },
		(Test){ TestTranspose_ColumnBlock<8>, "Read columns, blocks of 8" },
		(Test){ TestTranspose_ColumnBlock<16>, "Read columns, blocks of 16" },

		(Test){ TestTranspose_WriteBlock<4>, "Write blocks of 4" },
		(Test){ TestTranspose_WriteBlock<8>, "Write blocks of 8" },
		(Test){ TestTranspose_WriteBlock<16>, "Write blocks of 16" },

		//(Test){ TestTranspose_RowBlock2<2,2>, "Read rows, blocks 2x2" },
		//(Test){ TestTranspose_RowBlock2<4,2>, "Read rows, blocks 4x2" },
		//(Test){ TestTranspose_RowBlock2<8,2>, "Read rows, blocks 8x2" },
		//(Test){ TestTranspose_RowBlock2<16,2>, "Read rows, blocks 16x2" },
		//(Test){ TestTranspose_RowBlock2<2,4>, "Read rows, blocks 2x4" },
		//(Test){ TestTranspose_RowBlock2<4,4>, "Read rows, blocks 4x4" },
		//(Test){ TestTranspose_RowBlock2<8,4>, "Read rows, blocks 8x4" },
		//(Test){ TestTranspose_RowBlock2<16,4>, "Read rows, blocks 16x4" },
		//(Test){ TestTranspose_RowBlock2<2,8>, "Read rows, blocks 2x8" },
		//(Test){ TestTranspose_RowBlock2<4,8>, "Read rows, blocks 4x8" },
		//(Test){ TestTranspose_RowBlock2<8,8>, "Read rows, blocks 8x8" },
		//(Test){ TestTranspose_RowBlock2<16,8>, "Read rows, blocks 16x8" },
		//(Test){ TestTranspose_RowBlock2<2,16>, "Read rows, blocks 2x16" },
		//(Test){ TestTranspose_RowBlock2<4,16>, "Read rows, blocks 4x16" },
		//(Test){ TestTranspose_RowBlock2<8,16>, "Read rows, blocks 8x16" },
		//(Test){ TestTranspose_RowBlock2<16,16>, "Read rows, blocks 16x16" },
	};

	printf("Trialling with aligned matrices %ix%i\n", MATRIX_SIZE, MATRIX_SIZE );
	RunTests( tests );

	gData = (Data*)(void*)(dataAlloc+3);
	printf("Trialling with unaligned matrices %ix%i\n", MATRIX_SIZE, MATRIX_SIZE );
	RunTests( tests );

	free(dataAlloc);
	return 0;
}
