#!/usr/bin/bash

#submit for a given list of catalog queries

#top directory for all outputs
top="/gpfs01/star/pwg/jaroslav/test/star-upcDst/trees/BEMC_TOF_14"

#names for individual query outputs
qnames=("prod" "log" "mid" "high")

#catalog queries
qlist=(
  "production=P16id,trgsetupname=AuAu_200_production_2014,filetype=daq_reco_MuDst,filename~st_upc,storage=local,runnumber[]15078073-15167014"
  "production=P16id,trgsetupname=AuAu_200_production_low_2014,filetype=daq_reco_MuDst,filename~st_upc,storage=local,runnumber[]15078073-15167014"
  "production=P16id,trgsetupname=AuAu_200_production_mid_2014,filetype=daq_reco_MuDst,filename~st_upc,storage=local,runnumber[]15078073-15167014"
  "production=P16id,trgsetupname=AuAu_200_production_high_2014,filetype=daq_reco_MuDst,filename~st_upc,storage=local,runnumber[]15078073-15167014"
)

#macro to run the maker
macro="RunFilterMaker.C"

#scheduler xml
scheduler="scheduler.xml"

#-- end of config --


#directory with source codes, one level up
srcdir=$(cd ..; pwd)

#loop over catalog queries
i=0
while [ $i -lt ${#qnames[@]} ]
do
  echo $i

  #create jobs output folders
  basedir=$top"/"${qnames[$i]}
  mkdir -p $basedir"/logs"
  mkdir -p $basedir"/sched"

  #create xml for each query in the list
  xml="submit_"$i".xml"
  cat $scheduler | sed "s%__BASEDIR__%$basedir%g"\
                 | sed "s%__SRCDIR__%$srcdir%g"\
                 | sed "s%__MACRO__%$macro%g"\
                 | sed "s%__QUERY__%${qlist[$i]}%g" > $xml

  #submit for a given catalog query
  star-submit $xml

  sleep 1
  (( i++ ))
done
#loop over catalog queries













