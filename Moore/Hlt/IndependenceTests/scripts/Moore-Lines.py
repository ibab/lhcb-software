#!/usr/bin/env python
#
# Script to test independence of Hlt lines

import os, sys, subprocess, re, optparse

# Configuration
import Gaudi.Configuration
from Configurables import GaudiSequencer as Sequence
from Configurables import EventSelector, HltConf

from Moore.Configuration import Moore

# GaudiPython
from GaudiPython import AppMgr

def find_accept( startLine, lines ):
    """ Function to obtain an accept line from a list of output lines. """
    iter = lines.__iter__()
    while 1:
        try:
            line = iter.next()
            if re.search( "^" + startLine + "\s", line ):
                acceptIter = iter
                try:
                    for i in range( 6 ):
                        if re.search( r"#accept", line ):
                            return line
                        line = acceptIter.next()
                except StopIteration:
                    return None
        except StopIteration:
            return None
    return None

def main():

    # Setup the option parser
    usage = "usage: %prog [options] inputfile <inputfile>"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2015", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", type="int", action = "store", dest = "EvtMax",
                       default = 10000, help = "Number of events to run" )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='MC09-20090602', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'sim-20090402-vc-md100', help = "CondDBtag to use" )
    parser.add_option( "--settings", action = "store", dest="ThresholdSettings",
                       default = 'Physics_25ns_September2015',
                       help = "ThresholdSettings to use")
    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")
    parser.add_option( "--dbsnapshot", action = "store_true", dest = "UseDBSnapshot",
                       default = False, help = "Use a DB snapshot" )
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    parser.add_option( "--rch", action = "store_true", dest = "RunChangeHandler",
                       default = False, help = "Use the RunChangeHandler" )

    # Parse the arguments
    (options, args) = parser.parse_args()

    # Make sure there is data to run on

    # Put the options into the Moore configurable
    Moore().ThresholdSettings = options.ThresholdSettings

    #Moore().OutputLevel="VERBOSE" 
    Moore().EvtMax = options.EvtMax

    #Moore().UseDBSnapshot = options.UseDBSnapshot # DEPRECATED
    from Configurables import CondDB
    CondDB().UseDBSnapshot = options.UseDBSnapshot
    #
    Moore().DDDBtag   = options.DDDBtag
    Moore().CondDBtag = options.CondDBtag
    Moore().Simulation = options.Simulation
    Moore().DataType   = options.DataType
    Moore().inputFiles = args

    EventSelector().PrintFreq = 100

    # Instanciate the AppMgr to get the Hlt lines from their Sequences
    appMgr = AppMgr()

    hlt1Seq = Sequence( "Hlt1" )
    hlt1Lines = set()
    for m in hlt1Seq.Members:
        hlt1Lines.add( m.name() )

    hlt2Seq = Sequence( "Hlt2" )
    hlt2Lines = set()
    for m in hlt2Seq.Members:
        hlt2Lines.add( m.name() )

    # The AppMgr is no longer needed
    appMgr.exit()

    print "HLT1LINES"
    for line in hlt1Lines:
        print line
    print "HLT2LINES"
    for line in hlt2Lines:
        print line

if __name__ == "__main__":
    sys.exit( main() )
