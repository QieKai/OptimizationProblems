#!/bin/bash
i=1
for file in ` find $1 -name "*.d.out" | sort`
do
    bfile=$(basename $file)
    filename1=${bfile%.*}
    filename2=${filename1%.*}
    echo $filename2
    hfile="./graphs1/${filename2}.alist"
    ./verify_output $hfile < $file
    echo -e "\n"
    let i=i+1
done
