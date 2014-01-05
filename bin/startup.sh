#!/bin/bash 

b=""
for ((i=0;i<=6;i+=1))
do
   printf "begin start up the file2db, please wait some minute %s\r" $b
   sleep 0.3
   b+="."
done

printf "\n"
./file2db -i /sbilling/work/vince/work/file2db/cfg/csvToMydb.conf
