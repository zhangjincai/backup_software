#!/bin/bash

#while [ "$yn" != "yes" -o "$yn" != "YES" ]
until [ "$yn" == "yes" -o "$yn" == "YES" ]
do
	read -p "please input yes/YES to stop this program:" yn
done

echo "the answer you input is correct."
