#!/bin/bash

network="192.168.1"
for sitenu in $(seq 1 20)
do
	ping -c 1 -w 1 ${network}.${sitenu} &> /dev/null && result=0 || result=1
	if [ "$result" == 0 ];then 
		echo "server ${network}.${sitenu} is up"
	else
		echo "server ${network}.${sitenu} is down"
	fi
done
	
