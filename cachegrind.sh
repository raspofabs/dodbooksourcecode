#!/bin/sh

rm -f cachegrind.out.*
valgrind --tool=cachegrind --branch-sim=yes --log-file=cg_simple.out ./fsm_simple.out
valgrind --tool=cachegrind --branch-sim=yes --log-file=cg_oo.out ./fsm_oo.out
valgrind --tool=cachegrind --branch-sim=yes --log-file=cg_table.out ./fsm_table.out
valgrind --tool=cachegrind --branch-sim=yes --log-file=cg_tableptrs.out ./fsm_tableptrs.out
