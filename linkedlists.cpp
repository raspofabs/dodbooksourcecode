#include "common.h"

struct A {
	int val;
	int pad1;
	int pad2;
	int pad3;
};
struct Alink {
	Alink *next;
	int val;
	int pad1;
	int pad2;
	int pad3;
};

A *aArray;
A **aPointerArray;
Alink *aLinkedList;

const int ELEMENT_COUNT = 4 * 1024 * 1024;

void SetupValues() {
	// fill with data
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, 1234, 5678);
	Alink *link = aLinkedList;
	for( int i = 0; i < ELEMENT_COUNT; ++i ) {
		int value = pcg32_random_r_range(&rng, 0, 255 );
		aArray[i].val = value;
		aPointerArray[i]->val = value;
		link->val = value;
		link = link->next;
	}
}
void Setup() {
	Timer t;
	// allocate arrays
	aArray = (A*)malloc( sizeof(A) * ELEMENT_COUNT );

	aPointerArray = (A**)malloc( sizeof(A*) * ELEMENT_COUNT );

	// allocate elements
	for( size_t i = 0; i < ELEMENT_COUNT; ++i ) {
		Alink *link = (Alink*)malloc( sizeof(Alink) );
		link->next = aLinkedList;
		aLinkedList = link;
		aPointerArray[i] = (A*)malloc( sizeof(A) );
	}
	SetupValues();
	printf( "Setup took %fms\n", t.elapsed() );
}

int writeOut;
void UseVariable( int var ) {
	writeOut += var;
}

void TestSumArray() {
	int accumulator = 0;
	for( int i = 0; i < ELEMENT_COUNT; i+=1 ) {
		accumulator += aArray[i].val;
	}
	UseVariable( accumulator );
	//printf( "Acc %i\n", accumulator );
}
void TestSumArrayPointer() {
	int accumulator = 0;
	for( int i = 0; i < ELEMENT_COUNT; i+=1 ) {
		accumulator += aPointerArray[i]->val;
	}
	UseVariable( accumulator );
	//printf( "Acc %i\n", accumulator );
}
void TestSumLinkedList() {
	int accumulator = 0;
	Alink *link = aLinkedList;
	while( link != nullptr ) {
		accumulator += link->val;
		link = link->next;
	}
	UseVariable( accumulator );
	//printf( "Acc %i\n", accumulator );
}

int main() {
	Setup();

	{
		Test tests[] = {
			(Test){ TestSumArray, "Array Sum" },
			(Test){ TestSumArrayPointer, "Pointer Array Sum" },
			(Test){ TestSumLinkedList, "Linked List Sum" },
		};

		RunTests( tests );
	}

	return 0;
}
