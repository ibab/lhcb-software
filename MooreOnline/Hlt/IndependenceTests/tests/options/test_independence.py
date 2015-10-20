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
from HltMonitoring.Base import ProcessWrapper

from GaudiPython.Bindings import gbl

def index( seq, item ):
    """Return the index of the first item in seq."""
    for index in xrange( len( seq ) ):
        if seq[ index ] == item:
            return index

def remove(lines):
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
    return remove
    
def find_lines_quick_test( options, args ):
        
    hlt1Lines = set(["Hlt1TrackMVA","Hlt1TwoTrackMVA","Hlt1Lumi","Hlt1TrackMuon"])
    hlt2Lines = set()
    
    to_remove = remove(hlt1Lines.union(hlt2Lines)) # list
    for line in to_remove: 
        if line in hlt1Lines:
            hlt1Lines.remove( line )
        if line in hlt2Lines:
            hlt2Lines.remove( line )
    return hlt1Lines, hlt2Lines, to_remove


def find_lines( options, args ):
        
    jobArgs = ['--settings',options.Settings,
               '--dddbtag', options.DDDBtag,
               '--conddbtag', options.CondDBtag]
    
    print options
    
    command = os.path.expandvars( '$INDEPENDENCETESTSROOT/scripts/Moore-Lines.py')

    p = subprocess.Popen( [command] + jobArgs, stdout = subprocess.PIPE,
                          stderr = subprocess.STDOUT )
    
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

    to_remove = remove(hlt1Lines.union(hlt2Lines)) # list
    for line in to_remove: 
        if line in hlt1Lines:
            hlt1Lines.remove( line )
        if line in hlt2Lines:
            hlt2Lines.remove( line )
    return hlt1Lines, hlt2Lines, to_remove

def run( options, args ):

    print 'Starting run method'
    
    EventSelector().Input = ["DATAFILE='PFN:mdf:root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/2015ValidationData/Run_0163051_20150913-213332.hlta0101.mdf' SVC='LHCb::MDFSelector' OPT='READ'"]
    EventSelector().PrintFreq = 1

    from Configurables import Moore
    from Configurables import CondDB
    from Configurables import L0Conf, HltConf

    # Put the options into a dictionary
    reporterConfig = dict()
    reporterConfig[ 'ThresholdSettings' ] = options.Settings 
    reporterConfig[ 'EvtMax' ] = options.EvtMax
    reporterConfig[ 'DDDBtag' ] = options.DDDBtag
    reporterConfig[ 'CondDBtag' ] = options.CondDBtag
    reporterConfig[ 'Simulation' ] = options.Simulation
    reporterConfig[ 'DataType' ] = options.DataType
    reporterConfig[ 'Input' ]  = EventSelector().Input
    reporterConfig[ 'Input' ]  = EventSelector().Input
    # fixed configurations
    reporterConfig[ 'Wait' ] = False     ## Don't wait after each event
    reporterConfig[ 'Persistency']= 'ROOT';
    reporterConfig[ 'EnableRunChangeHandler']= True;
    reporterConfig[ 'WriterRequires']= [];
    reporterConfig[ 'WriteFSR']= False;
    reporterConfig[ 'EnableDataOnDemand']= True;
    reporterConfig[ 'CheckOdin']= False;
    reporterConfig[ 'Split']= 'Hlt1';
    reporterConfig[ 'RemoveInputHltRawBanks'] = True
    reporterConfig[ 'UseTCK']= False;
    reporterConfig[ 'Simulation']= False;
    reporterConfig[ 'OutputLevel']= 3
    reporterConfig[ 'ForceSingleL0Configuration ']= False
    reporterConfig[ 'Catalogs' ] = FileCatalog().Catalogs

    # Find all lines in this configuration
    print 'Determining available lines'
    all1Lines, all2Lines,removedLines = find_lines( options, args )
    print all1Lines, all2Lines, removedLines
    ## no Hlt2 lines for now !!!!
    all2Lines = set()
    
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

    print 'starting rest of processes'

    wrappers[ 'allLines' ].start()
    running.add( 'allLines' )
        
    # keep track of the decisions
    dm = { 'pass'   : gbl.DecisionMap(),
           'all'    : gbl.DecisionMap(),
           'single' : gbl.DecisionMap() }

    # start the rest of the processes
    print 'starting %s of processes (total = %s)' %(options.NProcesses,len(wrappers.keys()))
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
    call = 0
    while True:
        call+=1
        if options.debug:
            print 'call %s' %call
        if options.debug:
            print 'running %s' %running
            print 'completed %s' %completed
            print 'todo %s' %todo
        if len( completed ) == len( wrappers ):
            print 'all jobs completed'
            print 'completed = %s' %len(completed)
            print 'requested = %s' %len(wrappers)
            break
        ### still jobs running
        ready = None
        try:
            ( ready, [], [] ) = select.select( [ wrappers[ name ] for name in running ], [], [] )
            for wrapper in ready:
                data = wrapper.getData( False )
                if options.debug:
                    print 'inspecting %s' %(wrapper.name())
                    print 'data:'
                    print data                    
                if data == None:
                    continue
                elif type( data ) == type( "" ):
                    if data == 'DONE':
                        completed.add( wrapper.name() )
                        wrapper.join()
                        if len( todo ) != 0:
                            to_start = todo.pop()
                            if options.debug:
                                print 'starting %s' %to_start
                            wrappers[ to_start ].start()
                            running.add( to_start )
                        running.difference_update( completed )
                else:
                    run = data.pop( 'run' )
                    event = data.pop( 'event' )
                    name = wrapper.name()
                    if name == 'allLines':
                        dm['pass'].addDecision( run, event, "dummy_line", True ) ## just count processed events
                        for line, dec in data.iteritems():
                            dm['all'].addDecision( run, event, line, dec )
                    else:
                        line = name+'Decision'
                        if line in data.keys():
                            dec = data[ line]
                            dm['single'].addDecision( run, event, line, dec )
        except select.error:
            sleep(5)
            print 'select.error'

    #### done with all processes
    #### now start to collect the results
    mismatches = defaultdict(set)
    
    print 'processed %s events' %len(dm['pass'].events())

    if options.debug:
        print 'pass events = %s' %len(dm['pass'].events())
        print 'all events = %s' %len(dm['all'].events())
        print 'single events = %s' %len(dm['single'].events())

    passed_all_not_single = {}
    passed_single_not_all = {}
    passed_single = {}
    passed_all = {}
    for line in hlt1Lines.union( hlt2Lines ):
        passed_all_not_single[line] = 0
        passed_single_not_all[line] = 0
        passed_single[line] = 0
        passed_all[line] = 0

    iEntry = -1
    for entry in dm['pass'].events():
        iEntry = iEntry + 1
        if options.debug:
            print 'Entry: %s/%s, [run,event] = [%s,%s]' %(iEntry,len(dm['pass'].events()),entry.first,entry.second)
        for line in hlt1Lines.union( hlt2Lines ):
            all_dec = dm['all'].decision( entry.first, entry.second, line+'Decision' )
            single_dec = dm['single'].decision( entry.first, entry.second, line+'Decision' )
            if options.debug:
                print '%s, Dec(all,single) = [%s,%s]' %(line,all_dec,single_dec)
            if all_dec != single_dec:
                  mismatches[ line ].add( entry )
            #####
            if single_dec: 
                passed_single[line] += 1
            if all_dec:
                passed_all[line] += 1
            if single_dec and not all_dec:
                passed_single_not_all[line] += 1
            if all_dec and not single_dec:
                passed_all_not_single[line] += 1
                  
    if len( mismatches ):
        print 'Found mismatches:'
        for line, events in mismatches.iteritems():
            print '%s %d' % ( line, len( events ) )
    else:
        print 'No mismatches found in %s' %len(dm['pass'].events())

    ### print a summary table
    first_width=36
    second_width=12
    full_width=76
    print '-'*full_width
    print ('Line:\t').expandtabs(first_width) + '|'+('All(A)\tSingle(S)\tA!S\tS!A').expandtabs(second_width)
    print '-'*full_width
    line_list = hlt1Lines.union( hlt2Lines )
    nLine=0
    for line in line_list:
        nLine+=1
        print ('%s:%s:\t'%(nLine,line)).expandtabs(first_width) + '|'+('%s\t%s\t%s\t%s' %(passed_all[line],passed_single[line],passed_all_not_single[line],passed_single_not_all[line])).expandtabs(second_width)
    print '-'*full_width
    print 'removed lines:', removedLines
    print '-'*full_width
    ### end print summary table
    
    return 0
    
if __name__ == "__main__":

    # Setup the option parser
    usage = "usage: %prog settings"
    parser = optparse.OptionParser( usage = usage )

    parser.add_option( "--DataType", action="store", dest="DataType", 
                       default="2015", help="DataType to run on.")
    
    parser.add_option( "-n", "--evtmax", action = "store", type = 'int',
                       dest = "EvtMax", default = 1e2, help = "Number of events to run" )
    
    parser.add_option( "--Settings", action = "store", dest = "Settings",
                       default = "Physics_25ns_September2015", help = "Threshold settings" )
    
    parser.add_option( "--nprocesses", action = "store", type = 'int',
                       dest = "NProcesses", default = 8,
                       help = "Number of simultaneous processes to run." )
    
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default="dddb-20150724", help="DDDBTag to use" )
    
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'cond-20150828', help = "CondDBtag to use" )

    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")
    
    parser.add_option( "--dbsnapshot", action = "store_true", dest = "UseDBSnapshot",
                       default = False, help = "Use a DB snapshot" )
    
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "all", help = "Colon seperated list of additional hlt1 lines" )
    
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "none", help = "Colon seperated list of additional hlt2 lines" )
    
    parser.add_option( "-d","--debug", action="store_true", dest="debug",
                       default=False, help="debug" )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    ## multiprocessing.log_to_stderr()
    ## logger = multiprocessing.get_logger()
    ## logger.setLevel(logging.INFO)

    #if not len( args ):
    #    print "No settings specified"
    #    exit( 1 )

    sys.exit( run( options, args ) )
