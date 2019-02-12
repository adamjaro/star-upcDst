#!/usr/bin/python

#--------------------------------------------------------------
# macro to submit upcDst production based on configuration file
#
# usage:
# ./SubmitPlugin.py config.in
#
#--------------------------------------------------------------

import os
from subprocess import Popen
import re
import sys
import time

from argument_parser import argument_parser

#_____________________________________________________________________________
if __name__ == "__main__":

    #name of config file from command line argument
    args = sys.argv
    args.pop(0)
    config = args.pop(0)

    #load parameters from config file
    parser = argument_parser()

    parser.add_parameter("top")
    parser.add_parameter("macro")
    parser.add_parameter("scheduler")
    parser.add_parameter("submit")
    parser.add_parameter("add_input", list)

    #default values for some parameters
    parser.set_default("macro", "RunFilterMaker.C") #macro to run the maker
    parser.set_default("scheduler", "scheduler_template.xml") #name of scheduler template macro
    parser.set_default("submit", "star-submit") #command to submit

    parser.parse(config)

    top = parser.get("top")
    macro = parser.get("macro")
    scheduler = parser.get("scheduler")
    submit = parser.get("submit")
    qlist = parser.get("add_input")

    #directory with config file is assumed the same as for this macro
    confdir = os.getcwd()

    #directory with source codes, one level up
    srcdir = confdir[:confdir.rfind("/")]

    print "Submitting upcDst production:"
    print "  Configuration from:               ", config
    print "  Directory with configuration file:", confdir
    print "  Source codes directory:           ", srcdir
    print "  Top directory for outputs:        ", top
    print "  Maker macro:                      ", macro
    print "  Scheduler xml template:           ", scheduler
    print "  Submit command:                   ", submit

    #prototypes for input from catalog query or from filelist
    in_query = ["<input URL=\"catalog:star.bnl.gov?", "\" nFiles=\"all\" />"]
    in_list = ["<input URL=\"filelist:", "\"/>"]

    #loop over catalog queries
    i=0
    for q in qlist:
        print "Submitting query #"+str(i), ":", q[0]

        #create jobs output folders
        basedir = top + "/" + q[0]
        Popen("mkdir -p "+basedir+"/logs", shell=True).communicate()
        Popen("mkdir -p "+basedir+"/sched", shell=True).communicate()

        #create xml for each query in the list
        xmlname = "submit_"+str(i)+".xml"
        xml = open(xmlname, "w")
        for line in open(scheduler, "r"):
            line = re.sub(r"__BASEDIR__", basedir, line)
            line = re.sub(r"__SRCDIR__", srcdir, line)
            line = re.sub(r"__MACRO__", macro, line)
            line = re.sub(r"__CONFIG__", config, line)
            line = re.sub(r"__CONFDIR__", confdir, line)
            if q[1].find(".list") < 0:
                #input is catalog query
                line = re.sub(r"__INPUT__", in_query[0]+q[1]+in_query[1], line)
            else:
                #input is from filelist
                line = re.sub(r"__INPUT__", in_list[0]+q[1]+in_list[1], line)
            xml.write(line)
        xml.close()

        #submit for a given catalog query
        Popen(submit+" "+xmlname, shell=True).communicate()

        i += 1
        #time.sleep(1)

    print "All done."




















