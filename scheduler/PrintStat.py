#!/usr/bin/python

from glob import glob
from subprocess import Popen, PIPE
import sys

#_____________________________________________________________________________
if __name__ == "__main__":

    basedir="/gpfs01/star/pwg/jaroslav/test/star-upcDst/trees/UPC_main_JpsiB_10_11_14/*"


    #command line argumets
    resubmit = False
    args = sys.argv
    while args != []:
        arg = args.pop(0)
        if arg == "-r": resubmit = True

    #submitted jobs
    joblist = []
    for job in glob(basedir + "/sched/*_*.csh"):
        joblist.append( job.split("sched/sched")[1].split(".csh")[0] )

    print "Submitted:", len(joblist)

    #running jobs
    running = []
    cmd = "condor_q -submitter jaroslav -format \"%s\\n\" CMD"
    out = Popen(cmd.split(), stdout=PIPE).communicate()[0].split("\n")
    for job in out:
        i1 = job.find("sched/sched")+len("sched/sched")
        i2 = job.find(".csh")
        if i2 < 0: continue
        jobid = job[i1:i2]
        #select jobs belonging to this production
        if jobid not in joblist:
            continue
        running.append(jobid)

    print "Running:", len(running)

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

    print "Errors:", len(missing)

    print "Done:", len(donelist)
    print "Output total size:", totsiz

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



















