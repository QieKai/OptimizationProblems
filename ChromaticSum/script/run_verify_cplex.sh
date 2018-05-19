#!/bin/bash
i=1
for file in ` find $1 -name "*.H" | sort`
do
  bfile=$(basename $file)
  echo ${bfile%.*}
  #./qubo_solver_cplex/qubo ${file%.*}.H |./verify_qubosolv $file
  ./qubo_solver_cplex/qubo $file |./verify_output ./graphs1/${bfile%.*}.alist

  echo -e "\n"
  let i=i+1
done
