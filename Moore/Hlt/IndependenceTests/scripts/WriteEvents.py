#!/usr/bin/env python
#
# Script to use several processes to write events with differing Hlt Decisions to a file
# The events to write are read from a file

# MultiProcessing
from multiprocessing import Process, Queue, Event, Condition, Lock
from time import sleep
import os, sys

# General imports
import random, optparse, subprocess

# GaudiPython
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
import GaudiPython
DecisionMap = GaudiPython.gbl.DecisionMap

# Local imports
from IndependenceTests.Tasks import EventWriter, EventReporter, time_string
from IndependenceTests.Base import ProcessWrapper

def run( options, args ):

    evtMax = options.EvtMax

    filename = args[ 0 ]
    args.remove( filename )

    if len ( args ):
        evtSel = EventSelector()
        evtSel.Input = []
        for f in args:
            descriptor = "DATAFILE='%s' SVC='LHCb::MDFSelector'" % f
            evtSel.Input.append( descriptor )
    
    # Put the options into a dictionary
    reporterConfig = dict()
    reporterConfig[ 'EvtMax' ] = options.EvtMax
    reporterConfig[ 'DDDBtag' ] = options.DDDBtag
    reporterConfig[ 'CondDBtag' ] = options.CondDBtag
    reporterConfig[ 'DataType' ] = options.DataType
    reporterConfig[ 'Input' ]  = EventSelector().Input
    reporterConfig[ 'Catalogs' ] = FileCatalog().Catalogs
        
    dm = DecisionMap()
    try:
        f = open( filename )
        for line in f:
            items = line.split()
            runNumber = int( items[ 0 ] )
            eventNumber = int( items[ 1 ] )
            dm.addDecision( runNumber, eventNumber, "any", 1 )
    finally:
        f.close()

    # Setup the EventReporter process
    reporterWrapper = ProcessWrapper( 0, EventReporter, 'reporter', reporterConfig )

    # Setup the output writer process
    writerConfig = dict()
    writerConfig[ 'EvtMax' ] = options.EvtMax
    writerConfig[ 'NPrevious' ] = options.NPrevious
    writerConfig[ 'Input' ]  = EventSelector().Input
    writerConfig[ 'Catalogs' ] = FileCatalog().Catalogs
    writerConfig[ 'Output' ] = "file:mismatches.raw"
    writerWrapper = ProcessWrapper( 1, EventWriter, 'writer', writerConfig )

    reporterWrapper.start()
    writerWrapper.start()

    n = 1
    while True:
        # acquire the writing condition to make sure the notify is not missed later
        writerWrapper.condition().acquire()
        # Get the decision reports from the processes
        done = False
        info = reporterWrapper.getData()
        if type( info ) == type( "" ):
            # Message
            if info == 'DONE':
                writerWrapper.putData( "DONE" )
                writerWrapper.condition().release()
                break

        # "Decode" the received info
        runNumber = info[ 0 ]
        eventNumber = info[ 1 ]

        # Determine whether we need to write this event
        write = False
        val = dm.decision( runNumber, eventNumber, "any" )
        if val: write = True

        # Let the writing process know what to do
        writerWrapper.putData( write )
        # Wait for the writing to complete
        writerWrapper.condition().wait()
        writerWrapper.condition().release()
        # Let the reporting processes run the next event
        if n % 500 == 1:
            print "Running event: " + str( n )
        n += 1

        condition = reporterWrapper.condition()
        condition.acquire()
        condition.notify()
        condition.release()

    # join all the processes
    reporterWrapper.join()
    writerWrapper.join()

    return 0
    
if __name__ == "__main__":

    # Setup the option parser
    usage = "usage: %prog [options] mismatches_file data_file <data_file...>"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2009", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", action = "store", type = 'int',
                       dest = "EvtMax", default = 1e4, help = "Number of events to run" )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='MC09-20090602', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'sim-20090402-vc-md100', help = "CondDBtag to use" )
    parser.add_option( "--nprevious", action = "store", dest = "NPrevious",
                       type = "int", default = 1, help = "Accept slow events" )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    ## multiprocessing.log_to_stderr()
    ## logger = multiprocessing.get_logger()
    ## logger.setLevel(logging.INFO)

    if len( args ) <= 1:
        print "No input files specified"
        exit( 1 )

    for arg in args:
        p = os.path.expandvars( arg )
        if not os.path.exists( p ):
           print "Error, input file " + p + " does not exist."
           exit( 1 )

    exit( run( options, args ) )
