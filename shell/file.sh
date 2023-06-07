#!/bin/bash
str=`ls`
for n in $str
do
	if [ -e $n ]
	then 
		if [ -r $n ]
		then
			echo "$n can be read"
		else
			echo "$n can not be read"
		fi
	fi
done
	
