#!/usr/bin/bash

##########Kong's condor script###########
#This is a version of submit script for using a filelist as input of your condor jobs:

#top directory for all outputs
top="/gpfs01/star/pwg/ztu/star-upcDst/scheduler_filelist/output"

#name of output job:
#folder of ${jobname} will be created
jobname="dAu200_UPC_Jpsi_4"

#filelist of your own:
#this will replace the __YOURFILELIST__ in your scheduler.xml (name would be modifed to submit_$jobname.xml)
filelist="/gpfs01/star/pwg/ztu/star-upcDst/txt/dAu200_MuDst_file_4.list"

#macro to run the maker
macro="RunFilterMaker.C"

#scheduler xml
scheduler="scheduler.xml"

#-- end of config --
#directory with source codes, one level up
srcdir=$(cd ..; pwd)

#create jobs output folders
basedir=$top"/"${jobname}
mkdir -p $basedir"/logs"
mkdir -p $basedir"/sched"

#create xml and use the "scheduler.xml" as a template and replace these
#following lines with what has specified in here
#for example, sed "s%__BASEDIR__%$basedir%g" is to replace "__BASEDIR__" with $basedir

xml="submit_"$jobname".xml"
cat $scheduler | sed "s%__BASEDIR__%$basedir%g"\
                 | sed "s%__SRCDIR__%$srcdir%g"\
                 | sed "s%__MACRO__%$macro%g"\
                 | sed "s%__YOURFILELIST__%$filelist%g" > $xml

#submit for a given catalog query
#One can check the $xml file before it gets submit by the next line:
#star-submit $xml

star-submit $xml

sleep 1













