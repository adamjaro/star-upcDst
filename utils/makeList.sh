#!/bin/bash
# A sample Bash script, by Trhlar

#get_file_list.pl -keys 'node,path,filename' -cond 'production=P20ic,trgsetupname=pp500_production_2017,filetype=daq_reco_picoDst,filename~st_rp,storage!=hpss' -limit 0 -delim "/" > picoDst.list

#sed -e 's/^/root:\/\//' -i picoDst.list

rm -r lists
mkdir lists

for run in "/star/data01/pwg_tasks/upc03/pp17/UPC_P20ic/"*
do
   touch lists/$(echo $run | cut -c 45-52).list
done


python prepare_lists.py picoDst.list

for list in "lists"/*
do
   sort -u $list -o $list
done
