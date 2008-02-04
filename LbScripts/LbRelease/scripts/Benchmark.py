#!/usr/bin/env python

import sys
from optparse import OptionParser
from threading import Thread
from subprocess import Popen

class TimedJob(Thread):
    id = 0
    def __init__(self, jobexe, joboptionfile, nbevent=None):
        # constructor of parent class
        Thread.__init__(self)
        self._id = TimedJob.id
        TimedJob.id += 1
        self._exename = jobexe
        self._optfile = joboptionfile
        self.setNumberOfEvents(nbevent)
        self.setOuputFile()
        # output data
        self._outfile = None        
        self._extime = None
        self._intime = None
        self._intevtusertime = None
        self._intevtclocktime = None
        self._intscale = None
    def setOuputFile(self, outputfile=None):
        if outputfile :
            self._outfile = outputfile
        else :
            self._outfile = jobexe + "%s" % self._id + ".log"
    def setNumberOfEvents(self, nbevent):
        self._nbevent = nbevent
    def run(self):
        outfile = open(self._outfile, "w")
        p = Popen([self._exename, self._optfile],
                  stdout=outfile,
                  stderr=outfile,
                  close_fds=True)
    def externalTime(self):
        return self._extime
    def internalTime(self):
        return self._intime


def runBench(jobexe, joboptionfile, nbevent=None, nbthread=1):
    job_threads = ( TimedJob(jobexe, joboptionfile, nbevent) for x in range(nbthread) )
    for s in job_threads :
        s.start()
    for s in job_threads :
        s.join()

if __name__ == '__main__':
    parser = OptionParser(usage=usage, version=version)
    parser.add_option("-n",
                      type="int",
                      dest="nbevent",
                      help="Number of Event to be processed")
    parser.add_option("-t", "--threads",
                      type="int",
                      dest="nbthread",
                      help="Number of parallel threads")
    parser.add_option("-o", "--output-dir",
                      dest="outputfile",
                      help="Initial random seed")
    

    options, args = parser.parse_args(sys.argv[1:])
    
    runBench(args[0], args[1], options.nbevent, options.nbthread)
    
