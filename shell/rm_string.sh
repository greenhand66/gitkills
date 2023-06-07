#!/bin/bash
str='hello320abc20world'
echo $str
echo "%: '${str%20*}'"
echo "%%: '${str%%20*}'"
echo "#: '${str#*20}'"
echo "##: '${str##*20}'"
str1='hello345abcde456world321hello123world35645hello'
echo "str1 ##: '${str1##*3[a-z]}'"
echo "str1 %:  '${str1%3[a-z]*}'"
