#!/bin/bash
declare -i num=$[ RANDOM ]%100+1
declare -i a=0
declare -i n=0
while [ ! $a -eq $num ]
do
	read -p "pls input a number:" a
	if [ $a -gt $num ]
	then 
		echo "too great"
	elif [ $a -lt $num ]
	then
		echo "too little"
	fi
	n=$n+1
done
echo "the num is $num,input $n times"
