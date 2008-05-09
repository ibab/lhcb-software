#!/usr/bin/env python

import sys
import os, re
from optparse import OptionParser
from threading import Thread
from subprocess import Popen, PIPE
from shutil import copy, rmtree
from time import time, sleep
from math import sqrt


class TimedJob(Thread):
    id = 0
    sourcelist = None
    def __init__(self, jobexe, joboptionfile, nbevent=0, randomseed=0, outputdir=None):
        # constructor of parent class
        Thread.__init__(self)
        self._id = TimedJob.id
        TimedJob.id += 1
        self._exename = jobexe
        self._optfile = ""
        self._workdir = "."
        self._startdir = os.getcwd()
        self._outfile = ""
        self._outdir = outputdir
        self.preparePool(joboptionfile, nbevent, randomseed)
        # output data
        self._startextime = None
        self._stopextime = None
        self._extime = None
        self._intime = 0.0
        self._intevtusertime = None
        self._intevtclocktime = None
        self._intevtmintime = None
        self._intevtmaxtime = None
        self._intscale = None
        self._sourcefile = None
        self._status = 0
    def setOuputFile(self):
        self._outfile = os.path.join(self._workdir,self._exename + ".log")
    def setNumberOfEvents(self, nbevent):
        self._nbevent = nbevent
        f = open(self._optfile,"a")
        f.write("\n ApplicationMgr.EvtMax     = %s;\n" % nbevent)
        f.close()
    def setRandomSeed(self, randomseed):
        self._randomseed = randomseed
        f = open(self._optfile,"a")
        f.write("\n RndmGenSvc.Engine.Seeds     = [ %s ] ; \n" % randomseed)
        f.close()
    def setOptFile(self,joboptionfile, nbevent, randomseed):
        self._optfile = os.path.join(self._workdir, "bench.opts")
        copy(joboptionfile, self._optfile)
        self.setNumberOfEvents(nbevent)
        self.setRandomSeed(randomseed)        
    def preparePool(self, joboptionfile, nbevent, randomseed):
        self._workdir = os.path.join(self._startdir, "pool_%04d" % self._id) 
        if os.path.exists(self._workdir):
            rmtree(self._workdir)
        os.mkdir(self._workdir)
        self.setOptFile(joboptionfile, nbevent, randomseed)
        self.setOuputFile()
    def getSourceFiles(self, sourcedir):
        flist = []
        p = Popen(["nsls", sourcedir],
                  stdout=PIPE, stderr=PIPE, close_fds=True)
        if not sourcedir.endswith("/"):
            sourcedir += "/"
        for line in p.stdout:
            fullname = sourcedir + line[:-1]
            flist.append(fullname)
        for line in p.stderr:
            print "Error:", line[:-1]
        retcode = os.waitpid(p.pid, 0)[1]
        return flist
    def setSourceFile(self, datasource, nbfiles=1):
        if TimedJob.sourcelist is None:
            TimedJob.sourcelist = self.getSourceFiles(datasource)
        self._sourcefile = TimedJob.sourcelist[self._id]
        shortfilename = self._sourcefile.split("/")[-1]
        f = open(self._optfile, "a")
        f.write("\nEventSelector.Input = {\n")
        f.write(" \"DATAFILE='PFN:%s' TYP='POOL_ROOT' OPT='READ'\"\n" % shortfilename)
        f.write("};\n")
        f.close()
        print "copying file %s to %s" % (self._sourcefile, self._workdir)
        p = Popen(["rfcp", self._sourcefile, self._workdir], stdout=PIPE, stderr=PIPE, 
                  close_fds=True)
        for line in p.stdout:
            print line[:-1]
        for line in p.stderr:
            print "Error:", line[:-1]
        retcode = os.waitpid(p.pid, 0)[1]
    def run(self):
        self._startextime = time()
        outfile = open(self._outfile, "w")
        p = Popen([self._exename, self._optfile],
                  cwd=self._workdir,
                  stdout=outfile,
                  stderr=outfile,
                  close_fds=True)
        self._status = os.waitpid(p.pid,0)[1]
        outfile.close()
        if self._status != 0 :
            print "there was a problem with the job in thread %s: status was %s" % (self._id, self._status)
        else :
            self._stopextime = time()
            self._extime = self._stopextime - self._startextime
            print "Job external event rate: %e (%d events in %e s)" % (self.externalRate(), self._nbevent, self._extime)
            self.getInternalTime()
            print "  Job internal event rate: %e (%d events in %e s)" % (self.internalRate(), self._nbevent, self._intime),
            print "    Event time: mean %f min %f max %f" % ( self._intevtclocktime, self._intevtmintime, self._intevtmaxtime) 
    def externalTime(self):
        return self._extime
    def externalRate(self):
        if self._extime :
            return self._nbevent/self._extime
        else :
            return 0.0 ;
    def internalTime(self):
        return self._intime
    def getInternalTime(self):
        _exponentexpr = "(e|E)[+-]?\d+"
        _fracexpr = "\.\d+"
        _intpartexpr = "\d+"
        _pointfloatexpr = "(%s)?%s|(%s)\." % (_intpartexpr, _fracexpr, _intpartexpr)
        _exponentfloatexpr = "(%s|%s)%s" % (_intpartexpr, 
                                            _pointfloatexpr, 
                                            _exponentexpr)
        _floatnumberexpr = "(%s|%s)" % (_pointfloatexpr, _exponentfloatexpr)
        thefile = open(self._outfile, "r")
        p = re.compile(r".+\s+INFO\s+EVENT\s+LOOP\s+\|\s*(?P<user>%s)\s*\|\s*(?P<clock>%s)\s*\|\s*(?P<min>%s)\s+(?P<max>%s)\s*\|\s*(?P<entries>%s)\s*\|\s*(?P<total>%s)"% (_floatnumberexpr, _floatnumberexpr, _floatnumberexpr, _floatnumberexpr, _intpartexpr, _floatnumberexpr))
        for l in thefile:
            m = p.search(l, 1)
            if m :
                self._intime = float(m.group("total"))
                self._intevtclocktime = float(m.group("clock"))
                self._intevtusertime = float(m.group("user"))
                self._intevtmaxtime  = float(m.group("max"))
                self._intevtmintime  = float(m.group("min"))
        thefile.close()
        return self._intime
    def internalRate(self):
        if self._intime :
            return self._nbevent/self._intime
        else :
            return 0.0
    def status(self):
        return self._status

        
class Bench:
    def __init__(self, jobexe, joboptionfile, nbevent=None, maxthreads=1):
        self._jobexe = jobexe
        self._startdir = os.getcwd()
        self._nbevent = nbevent


def RateMean(values):
    total = 0.0
    for x in values:
        total += x
    return total/len(values)

def RateMeanError(values):
    mean = RateMean(values)
    total = 0.0
    n = len(values)
    for x in values:
        total += (x-mean)*(x-mean)
    if n > 1 :
        total = total/((n-1)*n)
    return sqrt(total)

def totRate(values):
    return RateMean(values)*len(values)

def totRateError(values):
    return RateMeanError(values)*len(values)

def printResults(thread_list, nthr):
    totextrate = totRate([s.externalRate() for s in thread_list])
    totintrate = totRate([s.internalRate() for s in thread_list])
    totexterr = totRateError([s.externalRate() for s in thread_list])
    totinterr = totRateError([s.internalRate() for s in thread_list])
    nbthread = len(thread_list)
    print "Total external event rate for %s threads: %e +/- %e" % (nbthread, totextrate, totexterr)
    print "Total internal event rate for %s threads: %e +/- %e" % (nbthread, totintrate, totinterr)
    print "==================================================================================================="
    if nbthread and nbthread != nthr :
        ratio = float(nthr)/float(nbthread)
        totextrate = totextrate * ratio 
        totintrate = totintrate * ratio 
        totexterr = totexterr * ratio 
        totinterr = totinterr * ratio
        print "Corrected values:"
        print "Total external event rate for %s threads: %e +/- %e" % (nthr, totextrate, totexterr)
        print "Total internal event rate for %s threads: %e +/- %e" % (nthr, totintrate, totinterr)
        print "==================================================================================================="
        
        
    

def runBench(jobexe, joboptionfile, nbevent=None, nbthread=1, datasource=None, nbfiles=1):
    job_threads = [ TimedJob(jobexe, joboptionfile, nbevent, x) for x in range(nbthread) ]
    if datasource is not None:
        for s in job_threads :
            s.setSourceFile(datasource, nbfiles)
    sleep(10.0)
    print "Starting the threads ..."
    glstarttime = time()
    for s in job_threads :
        sleep(5.0)
        s.start()
    for s in job_threads :
        s.join()
    glstoptime = time()
    gltime = glstoptime - glstarttime
    if gltime :
        glrate = (nbthread*nbevent)/gltime
    else :
        glrate = 0.0
    print "==================================================================================================="
    print "Global event rate for %s threads: %e (%d events in %e s)" % (nbthread, glrate, nbthread*nbevent, gltime)
    print "==================================================================================================="
    print "Raw results:"
    printResults(job_threads, nbthread)
    valid_threads = [ x for x in job_threads if not x.status() ]
    print "Valid results:"
    printResults(valid_threads, nbthread)


def getNbProc():
    n = 0
    for line in open("/proc/cpuinfo", "r"):
        if line.startswith("processor"):
            n += 1
    return n

if __name__ == '__main__':

    usage = "%prog [options] exe optfile"
    version = "$Id: Benchmark.py,v 1.3 2008-05-09 14:23:27 hmdegaud Exp $"
    
    parser = OptionParser(usage=usage, version=version)
    parser.set_defaults(nbevent=1)
    parser.add_option("-n",
                      type="int",
                      dest="nbevent",
                      help="Number of Event to be processed")
    parser.set_defaults(nbthread=0)
    parser.add_option("-t", "--threads",
                      type="int",
                      dest="nbthread",
                      help="Number of parallel threads")
    parser.set_defaults(outputdir=".")
    parser.add_option("-o", "--output-dir",
                      dest="outputdir",
                      help="results output directory")
    parser.set_defaults(datasource=None)
    parser.add_option("-D", "--data",
                      dest="datasource",
                      help="input data directory")
    parser.set_defaults(nbfiles=1)
    parser.add_option("-N", "--number-of-files",
                      type = "int",
                      dest="nbfiles",
                      help="Number of input files per thread")
    

    options, args = parser.parse_args(sys.argv[1:])

    if options.nbthread == 0:
        options.nbthread = getNbProc()

    print "number of threads: %s" % options.nbthread

    
    runBench(args[0], args[1], options.nbevent, options.nbthread, options.datasource, options.nbfiles)
    
