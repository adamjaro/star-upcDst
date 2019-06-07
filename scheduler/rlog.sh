#!/usr/bin/bash

#reduce log to the first and last lines given by the number below
s1=$3

#original log and reduced output
log=$1
rlog=$2

echo "Reducing log: $log"
echo "Number of lines to keep: $s1"
echo "Reduced output $rlog"

#number of lines in the log file
nlin=$(wc -l $log | awk '{print $1}')

#line number to get the end of the log
s2=$(($nlin - $s1 + 1))

if [ "$s2" -gt 0 ] && [ "$s2" -lt "$s1" ]
then
  s2=$(($s1 + 1))
fi

#create the output
cat /dev/null > $rlog

#put the beginning
cat $log | sed -n "1,$s1 p" >> $rlog

#put the end of the original log
if [ "$s2" -gt 0 ]
then
  cat $log | sed -n "$s2,$nlin p" >> $rlog
fi



