#!/usr/bin/env python
#
# Script to obtain all Hlt Lines for a ThresholdSetting

import os, sys, optparse

# Configuration
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Moore.Configuration import Moore
from Configurables import GaudiSequencer as Sequence
from Configurables import EventSelector, HltConf

# GaudiPython
from GaudiPython import AppMgr

class TestConfig:
    """ A class to reconfigure to run with specified Hlt lines """
    def __init__( self, hlt1Lines, hlt2Lines ):

        # Always run the Globas lines to avoid crashes and monitoring problems
        if not "Hlt1Global" in hlt1Lines:
            hlt1Lines.append( "Hlt1Global" )

        if not "Hlt2Global" in hlt2Lines:
            hlt2Lines.append( "Hlt2Global" )

        self.hlt1Lines = hlt1Lines
        self.hlt2Lines = hlt2Lines

    def postConfigAction( self ):
        from HltLine.HltLine import hlt1Lines
        from HltLine.HltLine import hlt2Lines

        # Reconfigure the Hlt sequences
        lines1 = [ i for i in hlt1Lines() if  i.name() in self.hlt1Lines ]
        print "lines1 = " + str( lines1 )
        Sequence( "Hlt1" ).Members = [ i.configurable() for i in lines1 ]

        lines2 = [ i for i in hlt2Lines() if  i.name() in self.hlt2Lines ]
        print "lines2 = " + str( lines2 )
        Sequence( "Hlt2" ).Members = [ i.configurable() for i in lines2 ]

        # Reconfigure the monitoring to expect the correct lines
        HltConf().configureHltMonitoring( lines1, lines2 )

def main():

    # Setup the option parser
    usage = "usage: %prog [options] inputfile <inputfile>"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2009", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", action = "store", dest = "EvtMax",
                       default = 1e4, help = "Number of events to run" )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='MC09-20090602', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'sim-20090402-vc-md100', help = "CondDBtag to use" )
    parser.add_option( "--settings", action = "store", dest="ThresholdSettings",
                       default = 'Physics_10000Vis_1000L0_40Hlt1_Apr09',
                       help = "ThresholdSettings to use")
    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")
    parser.add_option( "--dbsnapshot", action = "store_true", dest = "UseDBSnapshot",
                       default = False, help = "Use a DB snapshot" )
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "", help = "Comma seperated list of additional hlt1 lines" )
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "", help = "Comma seperated list of additional hlt2 lines" )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    # Put the options into the Moore configurable
    Moore().ThresholdSettings = options.ThresholdSettings

    Moore().Verbose = options.Verbose
    Moore().EvtMax = options.EvtMax

    Moore().UseDBSnapshot = options.UseDBSnapshot
    Moore().DDDBtag   = options.DDDBtag
    Moore().CondDBtag = options.CondDBtag
    Moore().Simulation = options.Simulation
    Moore().DataType   = options.DataType
    Moore().inputFiles = args

    EventSelector().PrintFreq = 100

    # Put the comma separated lists of lines into lists
    hlt1Lines = []
    for line in options.Hlt1Lines.split( "," ):
        if ( len( line.strip() ) ):
            hlt1Lines.append( line )

    hlt2Lines = []
    for line in options.Hlt2Lines.split( "," ):
        if ( len( line.strip() ) ):
            hlt2Lines.append( line )

    # Instantiate the class to apply the required configuration
    config = TestConfig( hlt1Lines, hlt2Lines )
    appendPostConfigAction( config.postConfigAction )

    # Instantiate the AppMgr
    appMgr = AppMgr()

    # Make sure that we have Hlt lines to run
    if not len( Sequence( "Hlt1" ).Members ) or not len( Sequence( "Hlt2" ).Members ):
        print "error, no lines to run\n"
        return 2

    # Run the required number of events
    sc = appMgr.run( Moore().EvtMax )
    if sc.isFailure(): return 2

    # Done
    sc = appMgr.exit()
    if sc.isFailure():
        return 2
    else:
        return 0

if __name__ == "__main__":
    sys.exit( main() )
