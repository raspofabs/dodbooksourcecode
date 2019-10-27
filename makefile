USE_CLANG=0
ifeq ($(USE_CLANG),1)
CC=clang
LINK=-lm -lstdc++
else

ifeq ($(shell uname -s),Darwin)
	# brew install g++ libomp
	CC=$(shell whereis g++)
	FOPENMP=-Xpreprocessor -fopenmp -lomp
else
	CC=g++
	FOPENMP=-fopenmp
endif

COMPILE=-march=native

endif
SOURCE=$(wildcard *.cpp)
OUTS=$(SOURCE:.cpp=.out)
#ASSEMBLY=$(SOURCE:.cpp=.s)
TARGETS=$(OUTS) $(ASSEMBLY)
COMMON=common.h geom.h geom.o
ADDITIONAL=geom.o

CFLAGS=$(COMPILE) -O3 -DNDEBUG -std=c++11 -Wall -Wextra -Werror
#CFLAGS=$(COMPILE) -g -O0 -std=c++11 -Wall -Wextra -Werror
FLAGS=$(CFLAGS) $(FOPENMP) $(LINK)

all: $(TARGETS)

run: $(OUTS)
	./linkedlists.out

runall: $(OUTS)
	./basictheory.out
	./branch_prediction.out
	./cachesizeeffect.out
	./false_sharing.out
	./fsm.out
	./hotcold.out
	./KeyLookup.out
	./matrixtranspose.out
	./modifying_memory.out
	./readingtests.out
	./simd_test.out	
	./speculativewaste.out

%.out: %.cpp $(COMMON)
	$(CC) $(ADDITIONAL) $(FLAGS) -DTARGET=$* -o $@ $<

fsm.out: fsm.cpp $(COMMON)	FSM_OOState.h FSM_Simple.h \
														FSM_TableState.h FSM_OOImplicitState.h \
														FSM_VaryingTableState.h FSM_TableStatePointers.h \
														FSM_OOFunctional.h
	$(CC) $(ADDITIONAL) $(FLAGS) -DTARGET=$* -o fsm.out fsm.cpp

%.s: %.cpp
	$(CC) $(FLAGS) -S -g -o $@ $<

geom.o: geom.cpp geom.h
	$(CC) -c $(CFLAGS) -o geom.o geom.cpp

geom.out: geom.cpp
	touch geom.out

clean:
	rm -f *.out *.s *.o
	rm -f testdata*.csv
	rm -f testreport.txt

cleanreports:
	rm -f testdata*.csv
	rm -f testreport.txt
	rm -f cachegrind.out.*
	rm -f cg_*.out
