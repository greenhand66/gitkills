#!/bin/bash
func()
{
	echo "from signal $1"
	return 1
}

trap func SIGINT
read a
