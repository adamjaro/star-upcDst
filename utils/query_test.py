#!/usr/bin/python

from subprocess import check_output
from math import ceil

query = [
    "production=P16id,trgsetupname=AuAu_200_production_2014,filetype=daq_reco_MuDst,filename~st_upc,storage=",
    "production=P16id,trgsetupname=AuAu_200_production_low_2014,filetype=daq_reco_MuDst,filename~st_upc,storage=",
    "production=P16id,trgsetupname=AuAu_200_production_mid_2014,filetype=daq_reco_MuDst,filename~st_upc,storage=",
    "production=P16id,trgsetupname=AuAu_200_production_high_2014,filetype=daq_reco_MuDst,filename~st_upc,storage="
]

#storage="local"
storage="hpss"

get_list = ["get_file_list.pl", "-keys", "path,filename,size", "-limit", "0", "-cond"]

out = []
for i in query:
    out += check_output(get_list + [i+storage]).split("\n")

#for i in out: print i

#remove empty entries (last one is empty)
out = [i for i in out if i]

flist_all = []
totsiz = 0.
for i in out:
    il = i.split("::")
    flist_all.append( il[1] )
    totsiz += float(il[2])

#remove dupplicate files
flist = list( dict.fromkeys(flist_all) )

nall = len(flist_all)
nf = len(flist)

print "All files:", nall
print "After removing duplicates:", nf
print "Duplicate files removed:", nall-nf

print "Total size:", ceil(totsiz/1024**4), "T"

#print out
#print flist

#for i in flist: print i


