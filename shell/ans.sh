#!/bin/bash
runlevel=1
for s in /etc/rc$runlevel.d/K*
do
	level=${s#/etc/rc$runlevel.d/K*}
	echo $level
	level=${level%%[a-zA-Z]*}
	echo $level
	echo "-------"
done
