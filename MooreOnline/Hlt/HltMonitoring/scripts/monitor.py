#!/usr/bin/env python
#
# Script to create histograms from several HLT output files in parallel.
# General imports
import os, os.path, sys, optparse, re
from copy import copy
# For the exceptions
import Queue
import select

# Local imports
from HltMonitoring.Base import ProcessWrapper

def fmt(name) :
    _fmt = { 'RAW' : "DATAFILE='PFN:file:%s' SVC='LHCb::MDFSelector'"
           , 'MDF' : "DATAFILE='PFN:file:%s' SVC='LHCb::MDFSelector'"
           , 'DST' : "DATAFILE='PFN:file:%s' TYP='POOL_ROOTTREE' OPT='READ'"
           }
    return _fmt[ os.path.splitext( name )[ -1 ].lstrip( '.' ).upper() ] % name

def configureInput( run_info, n_processes, options ) :
    ## run_info[ 'year' ] = run_info[ 'startTime' ].split( '-' )[ 0 ]
    ## dirname = prefix + '/lhcb/data/%(year)s/RAW/FULL/LHCb/%(runType)s/%(runID)s' % run_info

    # Run on raw files from castor or daqarea
    prefix = { 'daqarea' : '/daqarea/lhcb/data/%s/RAW/%s/LHCb/COLLISION15' % (options.DataType, options.stream),
               'calib'   : '/calib/hlt/spillover',
               'nodes'   : '/net/hlt%s%02d%02d/localdisk/hlt1' }

    re_file = re.compile(r"(?:Run_)?(0*(%(runID)s)).*\.(mdf|raw)" % run_info)
    from itertools import product
    files = []
    base_dir = prefix[options.source]
    if base_dir.startswith("/net"):
        for node in product('abcdef', range(1, 6), range(1, 29)):
            bd = base_dir % node
            file_dir = bd
            if not os.path.exists(bd):
                continue
            if str(run) in os.listdir(bd):
                file_dir = os.path.join(bd, str(run))
            node_files = sorted([os.path.join(file_dir, f) for f in os.listdir(file_dir) if re_file.match(f)])
            print 'Added %d new files from node hlt%s%02d%02d' % tuple([len(node_files)] + list(node))
            files += node_files
            if options.NFiles > 0 and len(files) > options.NFiles:
                break
    else:
        file_dir = base_dir
        if str(run_info['runID']) in os.listdir(base_dir):
            file_dir = os.path.join(base_dir, str(run_info['runID']))
        files += sorted([os.path.join(file_dir, f) for f in os.listdir(file_dir) if re_file.match(f)])

    if not files:
        print "Could not find files in %s" % prefix[options.source]
        return files

    lists = [[] for i in xrange(n_processes)]
    for i, f in enumerate(files):
        index = i % n_processes
        lists[index].append(fmt(f))

    ## low = 0
    ## rest = n_files % n_processes
    ## for i in range( n_processes ):
    ##     up = low + min_length
    ##     if rest != 0:
    ##         up += 1
    ##         rest -= 1
    ##     lists.append( [ fmt( os.path.join(dirname,f) ) for f in files[ low : up ] ] )
    ##     low = up
    return lists

def run( options, args ):
    from HltMonitoring import Utils

    run_nr = int( args[ 1 ] )
    filename = "monitor_%d.root" % run_nr

    n_processes = options.Processes

    run_info = Utils.run_info( run_nr )

    input_lists = configureInput( run_info, n_processes, options )
    if not input_lists:
        return -1

    evtMax = -1
    if options.EvtMax != -1:
        evtMax = options.EvtMax / n_processes

    config = dict()
    config[ 'EvtMax' ] = evtMax
    config[ 'DDDBtag' ] = options.DDDBtag
    config[ 'CondDBtag' ] = options.CondDBtag
    config[ 'DataType' ] = options.DataType
    config[ 'Run' ]  = run_nr

    from HltMonitoring import Monitors

    monitors = dict( [ ( n, None ) for n in args[ 0 ].split( ':' ) ] )

    for mon in monitors.keys():
        try:
            monitors[ mon ] = getattr( Monitors, mon )
        except AttributeError:
            print "Unknown monitor type: ", mon
            return -1

    monitor = None
    combined = False
    if len( monitors ) == 1:
        monitor = monitors.items()[ 0 ][ 1 ]( config )
    else:
        monitor = getattr( Monitors, "Combined" )
        config[ 'monitors' ] = copy( monitors )
        monitor = monitor( config )
    config.update( monitor.config() )

    wrappers = []
    running = {}

    for i in xrange( n_processes ):
        cfg = copy( config )
        cfg[ 'Input' ]  = input_lists[ i ]
        # Put the options into a dictionary
        wrappers += [ ProcessWrapper( i, monitor.child_type(), 'monitor_%d' % i, cfg, options.Debug ) ]

    if not options.Debug:
        cwd = os.path.realpath( os.path.curdir )
        dirname = os.path.join( cwd, '%d' % run_nr )
        if not os.path.exists( dirname ):
            os.mkdir( dirname )

    for wrapper in wrappers:
        wrapper.condition().acquire()
        wrapper.start()

    for wrapper in wrappers:
        wrapper.condition().wait()
        wrapper.condition().release()
        print "Initialised process ", wrapper.key()
        running[ wrapper.key() ] = wrapper

    from ROOT import TFile
    root_file = TFile( filename, 'recreate' )
    hists = {}

    n_evt = 0
    counter = 0
    while True:
        ready = None
        try:
            ( ready, [], [] ) = select.select( running.values(), [], [] )

            for wrapper in ready:
                info = wrapper.getData( False )
                if type( info ) == type( '' ):
                    # Message
                    if info == 'DONE':
                        key = wrapper.key()
                        print "Process %d is done" % key
                        running.pop( key )
                        continue

                n_evt += 1
                counter += 1
                # "Decode" the received info
                monitor.fill( info )

                if counter >= 10000:
                    print "Received event %d" % n_evt
                    counter = 0

            if len( running ) == 0:
                print "Done; processed %d events" % n_evt
                break

        except select.error:
            continue

    # join all the processes
    for wrapper in wrappers:
        wrapper.join()

    histograms = {}
    if len( monitors ) == 1:
        histograms[ monitors.keys()[ 0 ] ] = monitor.histograms()
    else:
        histograms = monitor.histograms()

    for name in monitors.keys():
        root_dir = root_file.mkdir( name )
        for histo in histograms[ name ].values():
            root_dir.WriteObject( histo, histo.GetName() )

    root_file.Close()

    return 0

if __name__ == "__main__":

    # Setup the option parser
    usage = """usage: %prog [options] monitor_type run_nr
Where monitor type is a colon separated list of monitors;
Rate, Vertex and Mass are currently supported options.

For example:
$> monitor.py --nprocesses=4 -n 10000 Rate:Mass:Vertex 87880
"""

    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-s", "--source", action="store", dest="source",
                       default="daqarea", help="Data source, daqarea or castor.")
    parser.add_option( "-d", "--datatype", action="store", dest="DataType",
                       default="2015", help="DataType to run on.")
    parser.add_option( "--nfiles", action = "store", type = 'int',
                       dest = "NFiles", default = -1, help = "Total number of files to run." )
    parser.add_option( "-n", action = "store", type = 'int',
                       dest = "EvtMax", default = -1, help = "Total number of events to run" )
    parser.add_option( "--dddbtag", action="store", dest="DDDBtag",
                       default='head-20110302', help="DDDBTag to use" )
    parser.add_option( "--conddbtag", action = "store", dest = "CondDBtag",
                       default = 'head-20110308', help = "CondDBtag to use" )
    parser.add_option( "--nprocesses", action = "store", type = "int", dest = "Processes",
                       default = 4, help = "Number of parallel processes to run" )
    parser.add_option( "--stream", action = "store", dest = "stream",
                       default = "FULL", help = "Which stream to run on" )
    parser.add_option( "--debug", action = "store_true", dest = "Debug",
                       default = False, help = "Put output of all processes on stdout." )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    if options.source not in [ 'daqarea', 'castor', 'calib', 'nodes' ]:
        print "Invalid data source: %s" % options.source

    if options.stream not in [ 'FULL', 'TURBO', 'TURCAL', 'BEAMGAS' ]:
        print "Invalid data stream: %s" % options.stream

    if len( args ) != 2:
        print parser.usage
        exit( 1 )

    exit( run( options, args ) )
