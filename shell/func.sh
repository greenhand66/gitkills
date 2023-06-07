#!/bin/bash
func()
{
	##declare -i n
	for n in `seq $1 $2`
	do
		echo $n
	done
}
func $1 $2
	
