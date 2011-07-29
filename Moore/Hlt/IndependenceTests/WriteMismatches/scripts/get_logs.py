#!/usr/bin/env ganga
#
# Script to test independence of Hlt lines

import os, sys, subprocess, re, optparse, tarfile
from time import sleep

def index( seq, item ):
    """Return the index of the first item in seq."""
    for index in xrange( len( seq ) ):
        if seq[ index ] == item:
            return index

def finalize_jobs( jobs, debug = True, remove = False ):
    """ Function to finalize jobs, if debug is True, stdout and stderr are added to a tar file. """
    for job in jobs.values():
        if job.status != "completed"  and job.status != "failed":
            job.kill()

    if debug:
        tar = tarfile.open("debug.tar.bz2", "w:bz2")
        for name, job in jobs.items():
            out = job.outputdir + "stdout"
            out_name = name + "_stdout"
            err = job.outputdir + "stderr"
            err_name = name + "_stderr"
            xml = job.outputdir + "summary.xml"
            xml_name = name + "_summary.xml"
            tar.add( out, out_name )
            tar.add( err, err_name )
            tar.add( xml, xml_name )
        tar.close()

    if remove:
        for job in jobs.values():
            job.remove()
        
def main( options ):

    jobIDs = jobs.ids( options.Start, options.End )

    #run the monitoring to update job statuses
    runMonitoring()

    hltJobs = dict()

    # match the jobs to the line they were running
    for jobID in jobIDs:
        job = jobs( jobID )
        if job.status != "completed":
            print "All jobs must have completed"
            return 1
        hltJobs[ job.name ] = job

    # Store the logs
    finalize_jobs( hltJobs, True, options.Remove )
    return
    
if __name__ == "__main__":

    usage = "usage: %prog id_start id_end"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-r", "--remove", action = "store_true", dest = "Remove",
                       default = False, help = "Remove completed jobs")
    
    (options, args) = parser.parse_args()

    if len( args ) != 2:
        print parser.usage
        sys.exit( 1 )

    options.Start = int( args[ 0 ] )
    options.End = int( args[ 1 ] )

    sys.exit( main( options ) )
