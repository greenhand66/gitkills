#!/bin/bash
declare -i n1
declare -i n2
declare -i num
##n1控制行数，n1范围从1到9
for n1 in `seq 1 9`
do 
##n2控制列数，范围从1到n1
	for n2 in `seq 1 $n1`
		do
			num=$n1*$n2
			echo -n " $n2*$n1 = $num "				
		done
  ##当一行打印完成后，就输出换行 	
                 echo -e '\n'
done
