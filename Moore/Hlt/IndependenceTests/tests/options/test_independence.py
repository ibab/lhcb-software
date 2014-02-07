#!/usr/bin/env python
#
# Script to use several processes to write events with differing Hlt Decisions to a file

# MultiProcessing
from multiprocessing import Process, Queue, Event, Condition, Lock
from time import sleep
from copy import copy
import os
import sys
import select
from collections import defaultdict

# General imports
import random, optparse, subprocess

# Gaudi configuration
from Gaudi.Configuration import *

# Local imports
from IndependenceTests.Tasks import DecisionReporter, time_string
from HltMonitor.Base import ProcessWrapper

from GaudiPython.Bindings import gbl

def index( seq, item ):
    """Return the index of the first item in seq."""
    for index in xrange( len( seq ) ):
        if seq[ index ] == item:
            return index

def remove( lines ):
    remove = []
    bad = set( [ 'Hlt1ErrorEvent', 'Hlt1ODINTechnical', 'Hlt1Tell1Error', 'Hlt1Incident',
                 'Hlt1Global', 'Hlt2Global', 'Hlt1Lumi', 'Hlt1VeloClosingMicroBias',
                 'Hlt1L0Any', 'Hlt1MBMicroBiasTStation',
                 'Hlt1MBNoBias', 'Hlt1LumiMidBeamCrossing', 'Hlt1CharmCalibrationNoBias' ] )
    for line in lines:
        if line.find( 'RateLimited' ) != -1: remove.append( line )
        elif line.find( 'PassThrough' ) != -1: remove.append( line )
        elif line.find( 'BeamGas' ) != -1: remove.append( line )
        elif line in bad: remove.append( line )
    for line in remove: lines.remove( line )
    return lines
    
def find_lines( options, args ):
    

    
    jobArgs = [ '-d', options.DataType,
                '-n', str( int(options.EvtMax) ),
                '--dddbtag', options.DDDBtag,
                '--conddbtag', options.CondDBtag,
                '--settings', options.Settings ] #args[ 0 ] ]

    command = os.path.expandvars( '$INDEPENDENCETESTSROOT/scripts/lines.sh' )

    if options.debug:
        print 'Popen'
    p = subprocess.Popen( [command] + jobArgs, stdout = subprocess.PIPE,
                          stderr = subprocess.STDOUT )
    if options.debug:
        print 'communicate'

    o = p.communicate()[ 0 ]
    if ( p.returncode ):
        print "failed finding lines"
        return p.returncode

    # Get the list of hlt lines from the output
    output = []
    for line in o.split( "\n" ):
        if len( line.strip() ):
            output.append( line )
        if options.debug:
            print line
    
    print 'finding Hlt1 lines'
    hlt1Lines = set()
    for m in output[ index( output, "HLT1LINES" ) + 1 : index( output, "HLT2LINES" ) ]:
        hlt1Lines.add( m )

    print 'finding Hlt2 lines'
    hlt2Lines = set()
    for m in output[ index( output, "HLT2LINES" ) + 1 : index( output, "ENDLINES" ) ]:
        hlt2Lines.add( m )
    
    return remove( hlt1Lines ), remove( hlt2Lines )

def run( options, args ):

    evtMax = options.EvtMax
    
    EventSelector().Input = ["DATA='castor:/castor/cern.ch/user/e/evh/131883/131883_0x0046_NB_L0Phys_00.raw' SVC='LHCb::MDFSelector'"]
    
    # Put the options into a dictionary
    reporterConfig = dict()
    reporterConfig[ 'EvtMax' ] = options.EvtMax
    #reporterConfig[ 'Verbose' ] = options.Verbose
    #reporterConfig[ 'UseDBSnapshot' ] = options.UseDBSnapshot
    #reporterConfig[ 'DDDBtag' ] = options.DDDBtag
    #reporterConfig[ 'CondDBtag' ] = options.CondDBtag
    #reporterConfig[ 'Simulation' ] = options.Simulation
    #reporterConfig[ 'DataType' ] = options.DataType
    #reporterConfig[ 'ThresholdSettings' ] = options.Settings #args[ 0 ]
    #reporterConfig[ 'Input' ]  = EventSelector().Input
    
    from Configurables import Moore
    from Configurables import CondDB
    from Configurables import L0Conf, HltConf
    #reporterConfig["UseTCK"] = False
    reporterConfig[ 'Wait' ] = False     ## Don't wait after each event
    
    reporterConfig[ 'Input' ]  = EventSelector().Input
    reporterConfig[ 'UseDBSnapshot']= False;
    reporterConfig[ 'Persistency']= 'ROOT';
    reporterConfig[ 'EnableRunChangeHandler']= False;
    reporterConfig[ 'WriterRequires']= [];
    reporterConfig[ 'WriteFSR']= True;
    reporterConfig[ 'EnableDataOnDemand']= True;
    reporterConfig[ 'CheckOdin']= False;
    reporterConfig[ 'InitialTCK']= '0x00790038';
    reporterConfig[ 'Simulation']= False;
    reporterConfig[ 'ThresholdSettings']="Commissioning_PassThrough"
    reporterConfig[ 'ForceSingleL0Configuration ']= False
    reporterConfig[ 'Catalogs' ] = FileCatalog().Catalogs
    from Configurables import L0Conf
    L0Conf().EnsureKnownTCK=False
    from Configurables import L0MuonAlg; L0MuonAlg( "L0Muon" ).L0DUConfigProviderType = "L0DUConfigProvider"
    
    # Find all lines in this configuration
    print 'Determining available lines'
    all1Lines, all2Lines = find_lines( options, args )
    
    hlt1Lines = set()
    # Process the options to setup Hlt lines to be run
    if options.Hlt1Lines == 'none':
        pass
    elif options.Hlt1Lines == 'all':
        hlt1Lines = copy( all1Lines )
    else:
        # Put the comma separated lists of lines into lists
        for line in options.Hlt1Lines.split( ";" ):
            if ( len( line.strip() ) ):
                hlt1Lines.add( line )
                
    hlt2Lines = set()
    if options.Hlt2Lines == 'none':
        pass
    elif options.Hlt2Lines == 'all':
        hlt2Lines = copy( all2Lines )
    else:
        for line in options.Hlt2Lines.split( ";" ):
            if ( len( line.strip() ) ):
                hlt2Lines.add( line )
                
    if options.Verbose:
        for line in hlt1Lines:
            print line
        for line in hlt2Lines:
            print line

    # Check if the specified lines exist
    allLines = all1Lines.union( all2Lines )
    for line in hlt1Lines.union( hlt2Lines ):
        if line not in allLines:
            print "Error, " + line + " is not a valid Hlt 1 or 2 line."
            print "Available lines:"
            for l in allLines:
                print l
            return 1
    del allLines

    wrappers = dict()
    i = 0
    # Setup the process with all lines
    # Setup the processes running a all Hlt lines
    allConfig = reporterConfig.copy()
    allConfig[ 'Hlt1Lines' ] = list( hlt1Lines )
    allConfig[ 'Hlt2Lines' ] = list( hlt2Lines )
    wrappers[ 'allLines' ] = ProcessWrapper( i, DecisionReporter, 'allLines', allConfig,
                                             options.Verbose )

    # Setup the processes running a single Hlt1 line
    for lineName in hlt1Lines:
        i += 1
        config = reporterConfig.copy()
        config[ 'Hlt1Lines' ] = [ lineName ]
        config[ 'Hlt2Lines' ] = []
        wrappers[ lineName ] = ProcessWrapper( i, DecisionReporter, lineName, config,
                                               options.Verbose )

    # Keep track of jobs that have completed
    completed = set()
    running = set()
    todo = set()

    # start the allLines process first
    
    print 'starting allLines process'
    wrappers[ 'allLines' ].start()
    running.add( 'allLines' )
        
    # keep track of the decisions
    dm = { 'pass'   : gbl.DecisionMap(),
           'all'    : gbl.DecisionMap(),
           'single' : gbl.DecisionMap() }

    # start the rest of the processes
    print 'starting rest of processes'
    for name, wrapper in wrappers.iteritems():
        if name == "allLines": continue
        if len( running ) == options.NProcesses:
            break
        wrapper.start()
        running.add( name )
    todo = set( wrappers.keys() ).difference( running )

    
    if options.debug:
        print 'running %s' %running
        print 'completed %s' %completed
        print 'todo %s' %todo
    
    while True:
        if len( completed ) == len( wrappers ):
            break
        ready = None
        try:
            ( ready, [], [] ) = select.select( [ wrappers[ name ] for name in running ], [], [] )
            for wrapper in ready:
                data = wrapper.getData( False )
                if data == None:
                    continue
                elif type( data ) == type( "" ):
                    if data == 'DONE':
                        completed.add( wrapper.name() )
                        wrapper.join()
                        if len( todo ) != 0:
                            to_start = todo.pop()
                            wrappers[ to_start ].start()
                            running.add( to_start )
                        running.difference_update( completed )
                else:
                    run = data.pop( 'run' )
                    event = data.pop( 'event' )
                    name = wrapper.name()
                    if name == 'allLines':
                        dm['pass'].addDecision( run, event, "dummy_line", True )
                        for line, dec in data.iteritems():
                            dm['all'].addDecision( run, event, line, dec )
                    else:
                        line = name+'Decision'
                        dec = data[ line]
                        if options.fake_rate > 0:
                            if random.random() < options.fake_rate: dec = True
                        dm['single'].addDecision( run, event, line, dec )
        except select.error:
            if options.debug:
                print 'sleeping'
            sleep( 5 )

    mismatches = defaultdict(set)
    
    print 'processed %s events' %len(dm['pass'].events())

    if options.debug:
        print 'pass events = %s' %len(dm['pass'].events())
        print 'all events = %s' %len(dm['all'].events())
        print 'single events = %s' %len(dm['single'].events())

    iEntry = -1
    for entry in dm['pass'].events():
        iEntry = iEntry + 1
        if options.debug:
            print '\n'
            print 'Entry: %s/%s' %(iEntry,len(dm['pass'].events()))
            print '%s %s' %(entry.first, entry.second) 
        for line in hlt1Lines.union( hlt2Lines ):
            all_dec = dm['all'].decision( entry.first, entry.second, line+'Decision' )
            single_dec = dm['single'].decision( entry.first, entry.second, line+'Decision' )
            if options.debug:
                print 'line = %s' %line
                print 'single_dec = %s' %(single_dec)
                print 'all_dec = %s' %(all_dec)
            if all_dec != single_dec:
                  mismatches[ line ].add( entry )

    if len( mismatches ):
        print 'Found mismatches:'
        for line, events in mismatches.iteritems():
            print '%s %d' % ( line, len( events ) )
    else:
        print 'No mismatches found.'

    return 0
    
if __name__ == "__main__":

    # Setup the option parser
    usage = "usage: %prog settings"
    parser = optparse.OptionParser( usage = usage )
    
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2012", help="DataType to run on.")
    
    parser.add_option( "-n", "--evtmax", action = "store", type = 'int',
                       dest = "EvtMax", default = 1e3, help = "Number of events to run" )
    
    parser.add_option( "--Settings", action = "store", dest = "Settings",
                       default = "Physics_September2012", help = "Threshold settings" )
    
    parser.add_option( "--nprocesses", action = "store", type = 'int',
                       dest = "NProcesses", default = 8,
                       help = "Number of simultaneous processes to run." )
    
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='head-20120413', help="DDDBTag to use" )
    
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'head-20120420', help = "CondDBtag to use" )

    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")
    
    parser.add_option( "--dbsnapshot", action = "store_true", dest = "UseDBSnapshot",
                       default = False, help = "Use a DB snapshot" )
    
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "Hlt1TrackAllL0;Hlt1TrackMuon", help = "Colon seperated list of additional hlt1 lines" )
    
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "none", help = "Colon seperated list of additional hlt2 lines" )
    
    parser.add_option( "--l0", action="store_true", dest="L0",
                       default=True, help="Rerun L0" )
    
    parser.add_option( "--debug", action="store_true", dest="debug",
                       default=False, help="debug" )

    parser.add_option( "--fake_rate", action = "store", type = 'float',
                       dest = "fake_rate", default = -1,
                       help = "Fraction of random dummy triggers to simulate non independence.")
    

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    ## multiprocessing.log_to_stderr()
    ## logger = multiprocessing.get_logger()
    ## logger.setLevel(logging.INFO)

    #if not len( args ):
    #    print "No settings specified"
    #    exit( 1 )

    sys.exit( run( options, args ) )
