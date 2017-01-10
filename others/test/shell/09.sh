#!/bin/bash

read -p "please input a directory:" dir
if [ "$dir" == ""];then
	echo "the $dir is not exist."
	exit 1
fi

filelist=$(ls $dir)
for filename in $filelist
do
	perm=""
	test -r "$dir/$filename" && perm="$perm readable"
	test -w "$dir/$filename" && perm="$perm writable"
	test -x "$dir/$filename" && perm="$perm execuable"
	echo "the file $dir/$filename's permission is $perm."
done
