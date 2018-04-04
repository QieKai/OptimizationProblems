#!/bin/bash
function chromaticSumQUBO(){
    for file in ` find $1 -name "*.alist"`
    do
      k=
        $2 $file
    done
}

CHRO_SUM_QUBO="../chromatic_sum_qubo"
CHRO_NUM_SOLV="../../ChromaticNum/lp_solver_cplex/lp_chromatic"
GRAPH_PATH="../graphs1"
chromaticSumQUBO $GRAPH_PATH $CHRO_SUM_QUBO $CHRO_NUM_SOLV
