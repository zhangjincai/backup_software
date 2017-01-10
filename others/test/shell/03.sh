#!/bin/bash

function printit(){
	echo -n "your choice is "
}

echo "this program will printf your selection!"

case $1 in
 "one")
	printit; echo $1 | tr 'a-z' 'A-Z'
	;;
 "two")
	printit; echo $1 | tr 'a-z' 'A-Z'
	;;
 "three")
	printit; echo $1 | tr 'a-z' 'A-Z'
	;;
 *)
	echo "usage $0 {one|two|three}"
	;;
esac
