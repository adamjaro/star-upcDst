#!/usr/bin/python

from glob import glob
from subprocess import Popen, PIPE
import sys
import os
from math import ceil

from argument_parser import argument_parser

#_____________________________________________________________________________
if __name__ == "__main__":

    #command line argumets
    resubmit = False
    args = sys.argv
    while args != []:
        arg = args.pop(0)
        if arg == "-r": resubmit = True
        elif arg == "-c": config = args.pop(0)

    #get outputs directory from config used for production
    parser = argument_parser()
    parser.add_parameter("top")
    parser.parse(config)
    basedir = parser.get("top") + "/*"

    print basedir

    #submitted jobs
    joblist = []
    for job in glob(basedir + "/sched/*_*.csh"):
        joblist.append( job.split("sched/sched")[1].split(".csh")[0] )

    print "Submitted:".ljust(10), len(joblist)

    #running jobs
    running = []
    cmd = "condor_q "+os.getlogin()
    out = Popen(cmd.split(), stdout=PIPE).communicate()[0].split("\n")
    for i in out:
        i1 = i.find("sched/sched")+len("sched/sched")
        i2 = i.find(".csh")
        if i2 < 0: continue
        jobid = i[i1:i2]
        #select jobs belonging to this production
        if jobid not in joblist: continue
        running.append(jobid)

    print "Running:".ljust(10), len(running)

    #done jobs
    donelist = []
    totsiz = 0
    #list all root files with size
    cmd = "ls -s " + basedir + "/*.root"
    out = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE).communicate()[0].split("\n")
    for fline in out:
        if len(fline) == 0: continue
        #get name and size for each file
        siznam = fline.lstrip().split(" ")
        size = int(siznam[0])
        name = siznam[1]
        #test for zero-sized outputs
        if size == 0:
            continue
        totsiz += size
        #job id from file name
        donelist.append(name.split("/")[-1][:-len(".root")])

    #missing jobs
    missing = []
    for job in joblist:
        if job in running or job in donelist:
            continue
        missing.append(job)

    print "Errors:".ljust(10), len(missing)

    print "Done:".ljust(10), len(donelist)
    print "Output total size:".ljust(20), totsiz, "K"
    print "".ljust(20), ceil(float(totsiz)/1024), "M"
    print "".ljust(20), ceil(float(totsiz)/1024**2), "G"

    #resubmit missing jobs if requested
    if resubmit is True and len(missing) > 0:
        print "Resubmitting the missing jobs"
        ijob = 0
        for job in missing:
            print "Job to resubmit:", job, "idx in list:", ijob
            session_num = job.split("_")
            cmd = "star-submit -r " + session_num[1] + " " + session_num[0] + ".session.xml"
            out = Popen(cmd.split(), stdout=PIPE, stderr=PIPE).communicate()
            print out[0]
            print out[1]
            ijob += 1
        print "Resubmittion done."



















