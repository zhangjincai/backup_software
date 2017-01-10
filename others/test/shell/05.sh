#!/bin/bash
s=0
i=0
until [ "$i" == "100" ]
do
	i=$(($i+1))
	s=$(($s+$i))
done
echo "1+2+3+...+100 = $s."
