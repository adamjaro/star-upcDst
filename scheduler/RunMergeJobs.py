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
    parser.add_parameter("outlist")
    parser.add_parameter("add_input", list)
    parser.set_default("outlist", "outlist.list") #macro to run the maker

    parser.parse(config)

    top = parser.get("top")
    qlist = parser.get("add_input")
    outlist = parser.get("outlist")

    #output files list
    chunk_list = open(top+"/"+outlist, "w")

    for q in qlist:
        merge_path = top + "/" + q[0]

        #list done jobs
        totsiz = 0    # total chunk size
        ichunk = 0    # chunk index
        tmpnam = "files.tmp"   # temporary to list files for a given chunk
        tmp = open(tmpnam, "w")
        cmd = "ls -s " + merge_path + "/*.root"  
        out = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE).communicate()[0].split("\n")

        #remove empty elements (last was empty string)
        out = [i for i in out if i]

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
            if iline == len(out)-1:
                tmp.close()
                print "Size:", totsiz
                chunkout = merge_path+"/"+ q[0]+".root"
                merge_cmd = "root -l -b -q MergeFiles.C(\"" + tmpnam + "\",\"" + chunkout + "\")"
                merg = Popen(merge_cmd.split(), stdout=PIPE, stderr=PIPE).communicate()
                print merg[0], merg[1]
                chunk_list.write(chunkout+"\n")
                #show the merged file
                os.system("ls -alh "+chunkout)
                os.system("chmod 444 " + chunkout)
                #reset the temporary and indices
                tmp = open(tmpnam, "r")
                for f in tmp:
						fname = f.rstrip()
						if os.path.isfile(fname): # this makes the code more robust
					  		os.remove(fname)
                totsiz = 0
        #list loop end

        #clean-up the list temporary file
        tmp.close()
        os.remove(tmpnam)

    print "All done."
























