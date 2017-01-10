#!/bin/bash

#input a birthday,output the days before the birthday

read -p "Please input your birthday (MMDD,eg:0709):" bir
now=`date +%m%d`

if [ "$bir" == "$now" ];then
	echo "Happy Birthday to you!!!"
elif [ "$bir" -gt "$now" ];then
	year=$((`date +%Y` + 1))
	total_d=$(($((`date --date="$year$bir" +%s`-`date +%s`))/60/60/24))
	echo "your birthday will be $total_d later."
#else 
#	offset=`expr $now - $bir`
	#total_d=`expr $(($((`date --date="$year$bir" +%s`-`date +%s`))/60/60/24)) - $offset`
#	total_d=`expr $total_d - $offset`
fi
