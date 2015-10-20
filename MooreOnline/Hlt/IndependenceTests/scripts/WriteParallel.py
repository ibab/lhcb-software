#!/usr/bin/env python
#
# Script to use several processes to write events with differing Hlt Decisions to a file

# MultiProcessing
from multiprocessing import Process, Queue, Event, Condition, Lock
from time import sleep
import os, sys

# General imports
import random, optparse, subprocess

# Gaudi configuration
from Gaudi.Configuration import *
from GaudiConf.Configuration import *

# Local imports
from IndependenceTests.Tasks import EventWriter, time_string

class ProcessData( object ):

    def __init__( self ):
        self._condition = Condition( Lock() )
        self._queue = Queue()
        pass

    def getData( self ):
        self._data = self._queue.get()
        return self._data

    def data( self ):
        return self._data

    def putData( self, data ):
        self._queue.put( data )

class ProcessWrapper( object ):

    def __init__( self, task, name, config ):
        self._name = name
        self._data = ProcessData()
        self._task = task( name, self._data._queue, self._data._condition )
        self._config = config
        self._process = Process( target = self.run )

    def run( self ):
        old_stdout = os.dup( sys.stdout.fileno() ) 
        fd = os.open( '%s.log' % self._name, os.O_CREAT | os.O_WRONLY ) 
        os.dup2( fd, sys.stdout.fileno() ) 
        self._task.configure( self._config )
        self._task.initialize()
        self._task.run()
        self._task.finalize()
        os.close( fd ) 
        os.dup2( old_stdout, sys.stdout.fileno() )

    def start( self ):
        self._process.start()

    def join( self ):
        self._process.join()

    def data( self ) :
        return self._data.data()

    def condition( self ):
        return self._data._condition

    def getData( self ):
        return self._data.getData()

    def putData( self, data ):
        self._data.putData( data )

def index( seq, item ):
    """Return the index of the first item in seq."""
    for index in xrange( len( seq ) ):
        if seq[ index ] == item:
            return index

def find_lines( options, args ):
    jobArgs = [ '-d', options.DataType,
                '-n', str( options.EvtMax ),
                '--dddbtag', options.DDDBtag,
                '--conddbtag', options.CondDBtag,
                '--settings', options.ThresholdSettings ]
    if options.Verbose: jobArgs += [ '-v' ]
    if options.Simulation: jobArgs += [ '-s' ]
    if options.UseDBSnapshot: jobArgs += [ '--dbsnapshot' ]

    p = subprocess.Popen( ['./lines.sh'] + jobArgs, stdout = subprocess.PIPE,
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

    hlt1Lines = set()
    for m in output[ index( output, "HLT1LINES" ) + 1 : index( output, "HLT2LINES" ) ]:
        hlt1Lines.add( m )

    hlt2Lines = set()
    for m in output[ index( output, "HLT2LINES" ) + 1 : index( output, "ENDLINES" ) ]:
        hlt2Lines.add( m )

    return list( hlt1Lines ), list( hlt2Lines )

def run( options, args ):

    evtMax = options.EvtMax

    if len( args ):
        EventSelector().Input = []
        for f in args:
            descriptor = "DATAFILE='%s' SVC='LHCb::MDFSelector'" % f
            EventSelector().Input.append( descriptor )

    # Put the options into a dictionary
    reporterConfig = dict()
    reporterConfig[ 'EvtMax' ] = options.EvtMax
    reporterConfig[ 'Verbose' ] = options.Verbose
    reporterConfig[ 'UseDBSnapshot' ] = options.UseDBSnapshot
    reporterConfig[ 'DDDBtag' ] = options.DDDBtag
    reporterConfig[ 'CondDBtag' ] = options.CondDBtag
    reporterConfig[ 'Simulation' ] = options.Simulation
    reporterConfig[ 'EnableAcceptIfSlow' ] = options.AcceptIfSlow
    reporterConfig[ 'DataType' ] = options.DataType
    reporterConfig[ 'ThresholdSettings' ] = options.ThresholdSettings
    reporterConfig[ 'Input' ]  = EventSelector().Input
    reporterConfig[ 'Catalogs' ] = FileCatalog().Catalogs
    ## Don't wait after each event
    reporterConfig[ 'Wait' ] = True
    
    # Put the comma separated lists of lines into lists
    hlt1Lines = []
    for line in options.Hlt1Lines.split( ";" ):
        if ( len( line.strip() ) ):
            hlt1Lines.append( line )

    hlt2Lines = []
    for line in options.Hlt2Lines.split( ";" ):
        if ( len( line.strip() ) ):
            hlt2Lines.append( line )

    # Find all lines in this configuration
    all1Lines, all2Lines = find_lines( options, args )

    # Check if the specified lines exist
    allLines = set( all1Lines + all2Lines )
    for line in hlt1Lines + hlt2Lines:
        if line not in allLines:
            print "Error, " + line + " is not a valid Hlt 1 or 2 line."
            print "Available lines:"
            for l in allLines:
                print l
            return 1
    del allLines

    # Hlt{1,2}Global always runs, so it can be removed here
    for lines in [ all1Lines, all2Lines, hlt1Lines, hlt2Lines ]:
        if "Hlt1Global" in lines: lines.remove( "Hlt1Global" )
        if "Hlt2Global" in lines: lines.remove( "Hlt2Global" )

    wrappers = dict()

    # Setup the process with all lines
    # Setup the processes running a all Hlt lines
    allConfig = reporterConfig.copy()
    allConfig[ 'Hlt1Lines' ] = all1Lines
    allConfig[ 'Hlt2Lines' ] = all2Lines
    wrappers[ 'allLines' ] = ProcessWrapper( 0, DecisionReporter, 'allLines', allConfig )

    # Setup the processes running a single Hlt1 line
    i = 0
    for lineName in hlt1Lines:
        i += 1
        config = reporterConfig.copy()
        config[ 'Hlt1Lines' ] = [ lineName ]
        config[ 'Hlt2Lines' ] = []
        wrappers[ lineName ] = ProcessWrapper( i, DecisionReporter, lineName, config )

    # Setup the processes running a single Hlt2 line
    for lineName in hlt2Lines:
        i += 1
        config = reporterConfig.copy()
        config[ 'Hlt1Lines' ] = all1Lines
        config[ 'Hlt2Lines' ] = [ lineName ]
        wrappers[ lineName ] = ProcessWrapper( i, DecisionReporter, lineName, config )

    # Setup the output writer process
    writerConfig = dict()
    writerConfig[ 'EvtMax' ] = options.EvtMax
    writerConfig[ 'NPrevious' ] = options.NPrevious
    writerConfig[ 'Input' ] = EventSelector().Input
    writerConfig[ 'Catalogs' ] = FileCatalog().Catalogs
    writerConfig[ 'Output' ] = "file:mismatches.raw"
    writerWrapper = ProcessWrapper( EventWriter, 'writer', writerConfig )

    # start the processes
    for wrapper in wrappers.values():
        wrapper.start()
    writerWrapper.start()

    n = 1
    while True:
        # acquire the writing condition to make sure the notify is not missed later
        writerWrapper.condition().acquire()
        # Get the decision reports from the processes
        done = False
        for ( name, wrapper ) in wrappers.items():
            sys.stdout.flush()
            report = wrapper.getData()
            if type( report ) == type( "" ):
                # Message
                if report == 'DONE':
                    done = True
                    break

        # FIXME, handle errors, this is not enough
        if done:
            writerWrapper.putData( "DONE" )
            writerWrapper.condition().release()
            break


        # Determine whether we need to write this event
        write = False
        reportsAll = wrappers[ 'allLines' ].data()
        for hltLine in hlt1Lines + hlt2Lines:
            reportsSingle = wrappers[ hltLine ].data()
            for name, decision in reportsSingle.items():
                if name.find( 'Global' ) != -1:
                    continue
                if reportsAll[ name ] != decision:
                    print "Mismatch: " + name
                    write = True


        # Let the writing process know what to do
        writerWrapper.putData( write )
        # Wait for the writing to complete
        writerWrapper.condition().wait()
        writerWrapper.condition().release()
        # Let the reporting processes run the next event
        if n % 50 == 1:
            print "Running event: " + str( n )
        n += 1
        for name, wrapper in wrappers.items():
            condition = wrapper.condition()
            condition.acquire()
            condition.notify()
            condition.release()

    # join all the processes
    for ( name, w ) in wrappers.items():
        w.join()
    writerWrapper.join()

    return 0
    
if __name__ == "__main__":

    # Setup the option parser
    usage = "usage: %prog [options] inputfile <inputfile>"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2009", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", action = "store", type = 'int',
                       dest = "EvtMax", default = 1e4, help = "Number of events to run" )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='MC09-20090602', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'sim-20090402-vc-md100', help = "CondDBtag to use" )
    parser.add_option( "--settings", action = "store", dest="ThresholdSettings",
                       default = 'Physics_320Vis_300L0_10Hlt1_Jan10',
                       help = "ThresholdSettings to use")
    parser.add_option( "-s", "--simulation", action = "store_true", dest = "Simulation",
                       default = False, help = "Run on simulated data")
    parser.add_option( "--dbsnapshot", action = "store_true", dest = "UseDBSnapshot",
                       default = False, help = "Use a DB snapshot" )
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    parser.add_option( "--acceptslow", action = "store_true", dest = "AcceptIfSlow",
                       default = False, help = "Accept slow events" )
    parser.add_option( "--nprevious", action = "store", dest = "NPrevious",
                       type = "int", default = 1, help = "Accept slow events" )
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "", help = "Colon seperated list of additional hlt1 lines" )
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "", help = "Colon seperated list of additional hlt2 lines" )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    ## multiprocessing.log_to_stderr()
    ## logger = multiprocessing.get_logger()
    ## logger.setLevel(logging.INFO)

    if not len( args ):
        print "No input file specified"
        exit( 1 )

    for arg in args:
        p = os.path.expandvars( arg )
        if not os.path.exists( p ):
           print "Error, input file " + p + " does not exist."
           exit( 1 )

    exit( run( options, args ) )
