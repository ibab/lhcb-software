#!/usr/bin/env ganga
#
# Script to test independence of Hlt lines

import os, sys, subprocess, re, optparse, tarfile
from time import sleep
from copy import copy

def index( seq, item ):
    """Return the index of the first item in seq."""
    for index in xrange( len( seq ) ):
        if seq[ index ] == item:
            return index

def available_datasets():
    """ Function to get the available LHCbDatasets and their names from the box """
    sets = dict()
    for o in box._impl.objects:
        typ = str( type( o ) ).split( "." )[ -1 ][ : -2 ]
        if typ == "LHCbDataset":
            sets[ o._getRegistry()._getName( o ) ] = o
    return sets
        
def main( options, args ):

    # Get the dataset from the box
    datasets = available_datasets()
    if len( args ) != 1:
        print "One dataset must be supplied"
        print "Available datasets:"
        for name in datasets.keys():
            print name
        return 1

    dataset = None
    try:
        dataset = datasets[ args[ 0 ] ]
    except KeyError:
        print "The requested dataset does not exist, available datasets:"
        for name in datasets.keys():
            print name

    # Site at which we run
    site = None
    platform = None
    backend = None
    splitter = None
    user_release_area = ""
    tempdir = "/tmp"

    # Check for splitting and maximum number of events
    if options.Split and options.EvtMax != -1:
        print "Warning, splitting requested and EvtMax != -1"
        sleep( 5 )
    
    # Set the backend and corresponding platform
    if options.Backend == "Local":
        platform = "x86_64-slc5-gcc43-opt"
        backend = Local()
        splitter = SplitByFiles( filesPerJob = options.FilesPerJob,
                                 maxFiles = options.MaxFiles )
        site = "NIKHEF.nl"
    elif options.Backend == "Dirac":
        backend = Dirac()
        platform = "x86_64-slc5-gcc43-opt"
        splitter = DiracSplitter( filesPerJob = options.FilesPerJob,
                                  maxFiles = options.MaxFiles )
        user_release_area = os.path.expandvars ( "$User_release_area" )
    elif options.Backend == "Stoomboot":
        backend = PBS( queue = 'stbcq', extraopts = '"-W group_list=bfys"' )
        platform = "x86_64-slc5-gcc43-opt"
        splitter = SplitByFiles( filesPerJob = options.FilesPerJob,
                                 maxFiles = options.MaxFiles )
        site = "NIKHEF.nl"
        tempdir = "/tmpdir"
    elif options.Backend == "LHCbT3":
        backend = LSF( queue = 'lhcbt3' )
        platform = "x86_64-slc5-gcc43-opt"
        splitter = SplitByFiles( filesPerJob = options.FilesPerJob,
                                 maxFiles = options.MaxFiles )
        site = "CERN.ch"
    elif options.Backend == "LSF":
        backend = LSF( queue = '8nh' )
        platform = "x86_64-slc5-gcc43-opt"
        splitter = SplitByFiles( filesPerJob = options.FilesPerJob,
                                 maxFiles = options.MaxFiles )
        site = "CERN.ch"
    else:
        print "Unknown backend specified, possibilities are:"
        print "Stoomboot"
        print "Dirac"
        print "Local"
        print "LHCbT3"
        print "LSF"
        return 1

    # Use the user specified directory if present
    if options.Tempdir != None:
        tempdir = options.Tempdir

    if options.UseDBSnapshot and options.UseOracle:
        print "DBSnapshot and Oracle are mutually exclusive"
        return 1

    if options.UseOracle and options.RunChangeHandler:
        print "Oracle and the RunChangeHandler are mutually exclusive"
        return 1

    # Settings
    Debug = True

    # job args needed to determine available lines
    linesArgs = [ options.Version, '-d', options.DataType,
                  '--settings', options.ThresholdSettings ]
    
    # Run the lines script to get the available Hlt lines
    print "Determining available lines"
    p = subprocess.Popen( ['./lines.sh'] + linesArgs, stdout = subprocess.PIPE,
                          stderr = subprocess.STDOUT )
    o = p.communicate()[ 0 ]
    if ( p.returncode ):
        print "failed to determine available Hlt lines"
        return p.returncode

    # Parse the output to get the lines
    output = []
    for line in o.split( "\n" ):
        if len( line.strip() ):
            output.append( line )

    hlt1Lines = set()
    availableHlt1Lines = set()
    for m in output[ index( output, "HLT1LINES" ) + 1 : index( output, "HLT2LINES" ) ]:
        availableHlt1Lines.add( m )

    hlt2Lines = set()
    availableHlt2Lines = set()
    for m in output[ index( output, "HLT2LINES" ) + 1 : ]:
        availableHlt2Lines.add( m )

    # Remove global, ratelimited and technical lines
    def remove( lines ):
        remove = []
        for line in lines:
            if line.find( 'RateLimited' ) != -1: remove.append( line )
            elif line.find( 'Global' ) != -1: remove.append( line )
        for line in remove: lines.remove( line )
        remove = []
        if not options.Technicals:
            for line in lines:
                if line.find( 'Lumi' ) != -1: remove.append( line )
                elif line.find( 'Bias' ) != -1: remove.append( line )
                elif line.find( 'Error' ) != -1: remove.append( line )
                elif line.find( 'Incident' ) != -1: remove.append( line )
                elif line.find( 'Beam' ) != -1: remove.append( line )
        for line in remove: lines.remove( line )
            
    remove( availableHlt1Lines )
    remove( availableHlt2Lines )

    jobArgs = [ '-d', options.DataType,
                '-n', str( options.EvtMax ),
                '--dddbtag', options.DDDBtag,
                '--conddbtag', options.CondDBtag,
                '--settings', options.ThresholdSettings,
                '--tempdir', tempdir ]

    #settings = { 'Verbose' : jobArgs += [ '-v' ],
    settings = { 'Verbose' :            [ '-v' ],
                 'Simulation' : [ '-s' ],
                 'UseDBSnapshot' : [ '--dbsnapshot', '--snd', options.SnapshotDirectory ],
                 'RunChangeHandler' : [ '--rch' ],
                 'FileStager' : [ '-f' ],
                 'L0' : [ '--l0' ] }

    for s, v in settings.iteritems():
        if getattr( options, s ):
            jobArgs += v

    if options.UseOracle:
        jobArgs += [ '--oracle' ]
        if site != None:
            jobArgs += [ '--site', site ]

    if len( options.VerboseClasses ): jobArgs += [ '-c', options.VerboseClasses ]

    # Process the options to setup Hlt lines to be run
    if options.Hlt1Lines == 'none':
        pass
    elif options.Hlt1Lines == 'all':
        hlt1Lines = copy( availableHlt1Lines )
    else:
        # Put the comma separated lists of lines into lists
        for line in options.Hlt1Lines.split( ";" ):
            if ( len( line.strip() ) ):
                if not line.strip() in availableHlt1Lines:
                    print "Warning, Hlt1 Line " + line.strip() + " is not available in this configuration, it is ignored."
                else:
                    hlt1Lines.add( line )

    if options.Hlt2Lines == 'none':
        pass
    elif options.Hlt2Lines == 'all':
        hlt2Lines = copy( availableHlt2Lines )
    else:
        for line in options.Hlt2Lines.split( ";" ):
            if ( len( line.strip() ) ):
                if not line.strip() in availableHlt2Lines:
                    print "Warning, Hlt2 Line " + line.strip() + " is not available in this configuration, it is ignored."
                else:
                    hlt2Lines.add( line )

    # Prepare the job with the full set of lines
    # Setup the command to run all lines
    runjob = 'Moore-RunJob.py'
    args = jobArgs[:]
    if len( hlt1Lines ):
        args += [ "--hlt1lines" ]
        lines = list( availableHlt1Lines )
        string = str( lines[ 0 ] )
        for line in lines[ 1: ]:
            string += ";" + line
        args += [ string ]
    if len( hlt2Lines ):
        args += [ "--hlt2lines" ]
        lines = list( availableHlt2Lines )
        string = str( lines[ 0 ] )
        for line in lines[ 1: ]:
            string += ";" + line
        args += [ string ]

    hltJobs = dict()

    print "Creating the jobs"
    # Setup the application
    gp = GaudiPython( script = 'Moore-RunJob.py',
                      project = 'Moore',
                      version = options.Version,
                      platform = platform,
                      user_release_area = os.environ[ 'User_release_area' ] ) 
    if options.FileStager or options.Oracle:
        gp.setupProjectOptions += ' --use-grid'
    gp.args = args

    inSandbox = [ 'Config.py' ]
    if options.FileStager:
        inSandbox += [ os.path.join( os.environ[ 'User_release_area' ], 'Moore_' + options.Version, 'InstallArea', platform, 'bin/garbage.exe' ) ]
    outSandbox = [ 'summary.xml', 'tuples.root' ]
    merger = SmartMerger( files = [ 'tuples.root', 'summary.xml' ] )
    print "allLines"
    job = Job( name = "allLines",
               application = gp,
               backend = backend,
               inputdata = dataset,
               inputsandbox = inSandbox,
               outputsandbox = outSandbox )
    if options.Split:
        job.merger = merger
        job.splitter = splitter
    hltJobs[ "allLines" ] = job

    # Prepare the jobs for individual Hlt lines
    for hlt1Line in hlt1Lines:
        args = jobArgs[:]
        args += [ "--hlt1lines", hlt1Line ]
        gp.args = args
        print hlt1Line
        job = Job( name = hlt1Line,
                   application = gp,
                   backend = backend,
                   inputdata = dataset,
                   inputsandbox = inSandbox,
                   outputsandbox = outSandbox )
        filename = "tuples.root"
        job.application.args += [ "--tuplefile=%s" % filename ]
        if options.Split:
            job.merger = merger
            job.splitter = splitter
        hltJobs[ hlt1Line ] = job
    for hlt2Line in hlt2Lines:
        args = jobArgs[:]
        # enable all hlt1 lines for each hlt2 line
        linesArg = ';'.join( availableHlt1Lines )
        args += [ "--hlt1lines", linesArg, '--hlt2lines', hlt2Line ]
        gp.args = args
        print hlt2Line
        job = Job( name = hlt2Line,
                   application = gp,
                   backend = backend,
                   inputdata = dataset,
                   inputsandbox = inSandbox,
                   outputsandbox = outSandbox )
        filename = "tuples.root"
        job.application.args += [ "--tuplefile=%s" % filename ]
        if options.Split:
            job.merger = merger
            job.splitter = splitter
        hltJobs[ hlt2Line ] = job

    i = 1
    n = len( hlt1Lines | hlt2Lines ) + 1
    submitQueue = set( hltJobs.keys() )
    nSubmitted = 0;

    print "Submitting jobs"

    if options.Submit:
        submitted = []
        for hltLine, job in hltJobs.items():
            job.submit()
            submitted.append( job.id )
            print "Submitted job " + str( i ) + "/" + str( n ) + ": " + hltLine
            i += 1
            submitQueue.remove( hltLine )
            nSubmitted += len( job.subjobs )
            if options.MaxJobs != -1:
                if nSubmitted >= options.MaxJobs: break
            if i % 5 == 4: runMonitoring()
        submitted.sort()
        print "Submitted jobs " + str( submitted[ 0 ] ) + "-" + str( submitted[ -1 ] )

    ids = []
    for job in hltJobs.values():
        ids.append( job.id )
    ids.sort()
    print "Created jobs " + str( ids[ 0 ] ) + "-" + str( ids[ -1 ] )

    return 0

if __name__ == "__main__":

    # Setup the option parser
    usage = "usage: %prog [options] dataset"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2009", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", type = "int", action = "store",
                       dest = "EvtMax", default = -1,
                       help = "Number of events to run" )
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
    parser.add_option( "--version", action = "store", dest = "Version",
                       default = 'v9r2', help = "Moore version to use" )
    parser.add_option( "--snd", action = "store", dest = "SnapshotDirectory",
                       default = '/user/graven/MOORE/conditions',
                       help = "DB Snapshot directory" )
    parser.add_option( "--oracle", action = "store_true", dest = "UseOracle",
                       default = False, help = "Use Oracle" )
    parser.add_option( "-v", "--verbose", action = "store_true", dest = "Verbose",
                       default = False, help = "Verbose output" )
    parser.add_option( "--acceptslow", action = "store_true", dest = "AcceptIfSlow",
                       default = False, help = "Accept slow events" )
    parser.add_option( "--nosubmit", action = "store_false", dest = "Submit",
                       default = True, help = "Submit created jobs" )
    parser.add_option( "--technicals", action = "store_false", dest = "Technicals",
                       default = True, help = "include technicals" )
    parser.add_option( "--backend", action = "store", type = "string",
                       dest = "Backend", default = "Stoomboot",
                       help = "submission backend" )
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "all",
                       help = "Colon seperated list of additional hlt1 lines." +
                       " Specifying all uses all available lines." )
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "all",
                       help = "Colon seperated list of additional hlt2 lines. "
                       + " Specifying all uses all available lines." )
    parser.add_option( "-c", "--verbose_classes", action = "store",
                       dest = "VerboseClasses", default = "",
                       help = "Comma seperated list of classes to be made verbose." )
    parser.add_option( "--rch", action = "store_true", dest = "RunChangeHandler",
                       default = False, help = "Use the RunChangeHandler" )
    parser.add_option( "--l0", action = "store_true", dest = "L0",
                       default = False, help = "Rerun L0" )
    parser.add_option( "-f", "--filestager", action = "store_true", dest = "FileStager",
                       default = False, help = "Use the filestager" )
    parser.add_option( "--split", action = "store_true", dest = "Split",
                       default = False, help = "Split jobs" )
    parser.add_option( "--maxjobs", type = "int", action = "store",
                       dest = "MaxJobs", default = 800,
                       help = "Max number of running jobs." )
    parser.add_option( "--maxfiles", type = "int", action = "store",
                       dest = "MaxFiles", default = -1,
                       help = "Max number of files to be used." )
    parser.add_option( "--filesperjob", type = "int", action = "store",
                       dest = "FilesPerJob", default = 5,
                       help = "Number of files per job." )
    parser.add_option( "--tempdir", type = "string", action = "store",
                       dest = "Tempdir", default = None,
                       help = "Temporary dir for the FileStager" )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    sys.exit( main( options, args ) )
