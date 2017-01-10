
#!/bin/bash

read -p "please input (Y/N):" yn
["$yn" == "Y" -o "$yn" == "y"] && echo "yes" && exit 0
["$yn" == "N" -o "$yn" == "n"] && echo "no" && exit 1
echo "i don't know" && exit -1
