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

def main( options, args ):
    
    ids = jobs.ids( int( args[ 0 ] ), int( args[ 1 ] ) )

    hltJobs = {}
    submitQueue = set()

    # Gather the jobs
    for id in ids:
        job = jobs( id )
        hltJobs[ job.name ] = job
        if job.status == "new":
            submitQueue.add( job.name )

    n = len( ids )
    i = n - len( submitQueue )
    
    # job args needed to determine available lines
    linesArgs = [ '--version', options.Version,
                  '-d', options.DataType,
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

    def remove( lines ):
        remove = []
        bad = set( [ 'Hlt1ErrorEvent', 'Hlt1ODINTechnical', 'Hlt1Tell1Error', 'Hlt1Incident',
                     'Hlt1Global', 'Hlt2Global', 'Hlt1Lumi', 'Hlt1VeloClosingMicroBias',
                     'Hlt1MBNoBias', 'Hlt1LumiMidBeamCrossing', 'Hlt1CharmCalibrationNoBias' ] )
        for line in lines:
            if line.find( 'RateLimited' ) != -1: remove.append( line )
            elif line in bad: remove.append( line )
        for line in remove: lines.remove( line )
    remove( availableHlt1Lines )

    hlt2Lines = set()
    availableHlt2Lines = set()
    for m in output[ index( output, "HLT2LINES" ) + 1 : ]:
        availableHlt2Lines.add( m )

    # Process the options to setup Hlt lines to be run
    if options.Hlt1Lines.lower() == 'none':
        pass
    elif options.Hlt1Lines == 'all':
        hlt1Lines = copy( availableHlt1Lines )
    else:
        # Put the comma separated lists of lines into lists
        for line in options.Hlt1Lines.split( ";" ):
            if ( len( line.strip() ) ):
                if not line.strip() in availableHlt1Lines:
                    print "Error, Hlt1 line " + line.strip() + " is not available."
                    return 1
                else:
                    hlt1Lines.add( line )

    if options.Hlt2Lines.lower() == 'none':
        pass
    elif options.Hlt2Lines == 'all':
        hlt2Lines = copy( availableHlt2Lines )
    else:
        for line in options.Hlt2Lines.split( ";" ):
            if ( len( line.strip() ) ):
                if not line.strip() in availableHlt2Lines:
                    print "Error, Hlt2 line " + line.strip() + " is not available."
                    return 1
                else:
                    hlt2Lines.add( line )

    print "Checking if jobs match requested lines."
    lines = hlt1Lines | hlt2Lines
    lines.add( "allLines" )
    jobNames = set( hltJobs.keys() ) 
    for line in lines:
        if line not in jobNames:
            print "Error, requested line " + line + " is not in the jobs."
            return 1


    # Keep track of jobs that have completed
    completedJobs = dict()

    # Run the monitoring to update status of jobs
    runMonitoring()

    nSubmitted = 0;
    for hltLine, job in hltJobs.items():
        if job.status == "completed":
            if len( job.subjobs ):
                completedJobs[ hltLine ] = set( job.subjobs.ids() )
            else:
                completedJobs[ hltLine ] = set()
        elif job.status != "submitted" and job.status != "running":
            continue
        if len( job.subjobs ) != 0:
            for sj in job.subjobs:
                if sj.status == "submitted" or job.status == "running":
                    nSubmitted += 1
                elif sj.status == "completed":
                    if hltLine in completedJobs.keys():
                        completedJobs[ hltLine ].add( sj.id )
                    else:
                        s = set()
                        s.add( sj.id )
                        completedJobs[ hltLine ] = s
        else:
            nSubmitted += 1

    print "Number of currently submitted jobs: " + str( nSubmitted )

    # Monitor the jobs
    print "Monitoring jobs"
    while True:
        runMonitoring()
        for hltLine, job in hltJobs.items():
            if len( job.subjobs ) != 0:
                # Job with subjobs
                if hltLine in completedJobs.keys() and len( completedJobs[ hltLine ] ) == len( job.subjobs ):
                    continue
                if job.status == "running" or job.status == "submitted":
                    keys = None
                    if not hltLine in completedJobs.keys():
                        keys = set()
                        completedJobs[ hltLine ] = keys
                    else:
                        keys = completedJobs[ hltLine ]
                    for sj in job.subjobs:
                        if sj.id in keys:
                            continue
                        elif sj.status == "completed":
                            nSubmitted -= 1
                            keys.add( sj.id )
                elif job.status == "completed":
                    completedJobs[ hltLine ] = set( job.subjobs.ids() )
                    nSubmitted -= 1
                elif job.status == "failed":
                    print "Job with line(s): " + hltLine + " failed." 
                    return 2
            else:
                if hltLine in completedJobs.keys():
                    continue
                # Job without subjobs
                if job.status == "completed":
                    completedJobs[ hltLine ] = set()
                    nSubmitted -= 1
                elif job.status == "failed":
                    print "Job with line(s): " + hltLine + " failed." 
                    return 2
        if  len( completedJobs.keys() ) == len( hltJobs ):
            print "Done!"
            break
        while ( len( submitQueue ) != 0 and nSubmitted < options.MaxJobs ):
            lineName = submitQueue.pop()
            j = hltJobs[ lineName ]
            j.submit()
            print "Submitted job " + str( i ) + "/" + str( n ) + ": " + lineName
            i += 1
            if len( j.subjobs ) != 0:
                nSubmitted += len( j.subjobs )
            else:
                nSubmitted += 1
        # Wait and run the monitoring to update job status.
        sleep( 10 )

    return 0

if __name__ == "__main__":

    # Setup the option parser
    usage = "usage: %prog [options] start_id end_id"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2010", help="DataType to run on.")
    parser.add_option( "--settings", action = "store", dest="ThresholdSettings",
                       default = 'Physics_10000Vis_1000L0_40Hlt1_Apr09',
                       help = "ThresholdSettings to use")
    parser.add_option( "--version", action = "store", dest = "Version",
                       default = 'v9r2', help = "Moore version to use" )
    parser.add_option( "--hlt1lines", action = "store", dest = "Hlt1Lines",
                       default = "all",
                       help = "Colon seperated list of additional hlt1 lines." +
                       " Specifying all uses all available lines." )
    parser.add_option( "--hlt2lines", action = "store", dest = "Hlt2Lines",
                       default = "all",
                       help = "Colon seperated list of additional hlt2 lines. "
                       + " Specifying all uses all available lines." )
    parser.add_option( "--maxjobs", type = "int", action = "store",
                       dest = "MaxJobs", default = 800,
                       help = "Max number of running jobs." )

    # Parse the command line arguments
    (options, args) = parser.parse_args()

    if len( args ) != 2:
        parser.print_usage()
        sys.exit( 1 )
    
    if options.MaxJobs < -1 or options.MaxJobs == 0:
        print "Bad max jobs value"
        sys.exit( 1 )

    try:
        sys.exit( main( options, args ) )
    except KeyboardInterrupt:
        sys.exit( 2 )
