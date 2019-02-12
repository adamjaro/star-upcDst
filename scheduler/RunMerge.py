#!/usr/bin/python

from glob import glob
from subprocess import Popen, PIPE
import os
import sys

from argument_parser import argument_parser

#_____________________________________________________________________________
if __name__ == "__main__":

    #merge output files by chunks of a given size

    #config file from command line argumet
    args = sys.argv
    args.pop(0)
    config = args.pop(0)

    parser = argument_parser()
    parser.add_parameter("top")
    parser.add_parameter("outdir")
    parser.add_parameter("outfile")
    parser.add_parameter("outlist")
    parser.add_parameter("add_input", list)

    parser.parse(config)

    top = parser.get("top")
    qlist = parser.get("add_input")

    #build pattern from query list
    pattern = "/{"
    for q in qlist: pattern += q[0]+","
    pattern = pattern[:-1] + "}/*.root"

    outdir = parser.get("outdir")
    outfile = parser.get("outfile")
    outlist = parser.get("outlist")

    print "Merge for:", top + pattern
    print "  Output directory:", outdir
    print "  Output file:     ", outfile
    print "  Output list:     ", outlist

    #size of one output chunk
    chunksiz = int(2.5e6)  # approx kB

    #create output directory for merged files
    merge_path = top+"/"+outdir
    if os.access(merge_path, os.F_OK) == False:
        os.makedirs(merge_path)

    #output files list
    chunk_list = open(merge_path+"/"+outlist, "w")

    #list done jobs
    totsiz = 0    # total chunk size
    ichunk = 0    # chunk index
    nchunk = 0    # number of files in chunk
    tmpnam = "files.tmp"   # temporary to list files for a given chunk
    tmp = open(tmpnam, "w")
    cmd = "ls -s " + top + pattern
    out = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE).communicate()[0].split("\n")
    print "Number of all outputs:", len(out)
    print
    #list loop
    for iline in xrange(len(out)):
        fline = out[iline]
        size = 0
        if len(fline) > 0:
            siznam = fline.lstrip().split(" ")
            size = int(siznam[0])
            name = siznam[1]
        if size > 0:
            totsiz += size
            tmp.write(name+"\n")
            nchunk += 1
        if totsiz >= chunksiz or iline == len(out)-1:
            tmp.close()
            print "Chunk:", ichunk
            print "Num files:", nchunk
            print "Size:", totsiz
            iform = "_{0:04d}".format(ichunk)
            chunkout = merge_path+"/"+outfile.split(".root")[0]+iform+".root"
            merge_cmd = "root -l -b -q MergeFiles.C(\"" + tmpnam + "\",\"" + chunkout + "\")"
            merg = Popen(merge_cmd.split(), stdout=PIPE, stderr=PIPE).communicate()
            print merg[0], merg[1]
            chunk_list.write(chunkout+"\n")
            #reset the temporary and indices
            tmp = open(tmpnam, "w")
            totsiz = 0
            nchunk = 0
            #increment the chunk index
            ichunk += 1
    #list loop end

    #clean-up the list temporary file
    tmp.close()
    os.remove(tmpnam)

    print "All done."
























