#!/usr/bin/env ganga
""" A script to create a list of tuples from a range of job ids. """

# imports
import sys, os, optparse

def main( start, end, filename ):
    """ main function """
    jobIDs = jobs.ids( start, end )

    #run the monitoring to update job statuses
    runMonitoring()

    files = dict()

    for j in jobIDs:
        job = jobs( j )
        if job.status != "completed":
            print "Job " + str( j ) + " has not completed"
            pass
        
        tf = job.outputdir + "tuples.root"
        if not os.path.exists( tf ):
            print "Job " + str( j ) + " did not produce tuples.root"
            return 1

        files[ job.name ] = tf

    f = open( filename, 'w' )
    for lineName, tf in files.items():
        line = lineName + " " + tf + "\n"
        f.write( line )
    f.close()
        
if __name__ == "__main__":
    
    usage = "usage: %prog [options] range_start range_end"
    parser = optparse.OptionParser( usage = usage )

    parser.add_option( "-f", "--filename", action="store",
                       dest="Filename", type = "string", default="tuples.lst",
                       help="File to write output tuple filenames to.")

    (options, args) = parser.parse_args()

    if len( args ) != 2:
        print parser.usage
        sys.exit( 1 )

    start = int( args[ 0 ] )
    end = int( args[ 1 ] )
    sys.exit( main( start, end, options.Filename ) )
