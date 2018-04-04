#!/bin/bash
function makeCSV(){
i=1
for file in ` find $1 -name "*.H" | sort`
do
  bfile=$(basename $file)
  alist="./graphs1/${bfile%.*}.alist"
  run1=`./verify_output -c $alist < $2/${bfile%.*}.d.out`
  run2=`./verify_output -c $alist < $3/${bfile%.*}.d.out`
  run3=`./verify_output -c $alist < $4/${bfile%.*}.d.out`
  run4=`./verify_output -c $alist < $5/${bfile%.*}.d.out`
  optimal=`./qubo_solver_cplex/qubo $file |./verify_output -ci $alist`
  bfile=`echo "$bfile" | tr ',' '_'`
  echo ${bfile%.*}, $optimal, $run1, $run2, $run3, $run4
  let i=i+1
done
}

HFILE="./dwave_output/QUBOs.chromatic+2"
RUN1="./dwave_output+2_opt/run1"
RUN2="./dwave_output+2_opt/run2"
RUN3="./dwave_output+2_opt/run3"
RUN4="./dwave_output+2_opt/run4"
echo Graph,Order,Size,K,Optimal Answer, Coloring Number,run1,Coloring Number,run2,Coloring Number,run3,Coloring Number,run4,Coloring Number
makeCSV $HFILE $RUN1 $RUN2 $RUN3 $RUN4
