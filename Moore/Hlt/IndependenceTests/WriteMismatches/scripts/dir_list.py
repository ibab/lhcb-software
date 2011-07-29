#!/usr/bin/env python
""" A script to check a directory for files named linename_tuples.root,
    a file containing a line for every file is created. A line contains
    Hlt line name and the filename, seperated by a whitespace. """

# imports
import sys, os, optparse

def main( filename, directory ):
    """ main function """
    l = os.listdir( directory )
    files = dict()
    for f in l:
        s = f.split( "_" )
        if s[ -1 ] == "tuples.root":
            line = s [ 0 ]
            if len( s ) != 2:
                for entry in s[ 0 : -1 ]: line += "_" + entry
            files[ line ] = directory + "/" + f

    f = open( filename, 'w' )
    for lineName, tf in files.items():
        line = lineName + " " + tf + "\n"
        f.write( line )
    f.close()
        
if __name__ == "__main__":
    
    usage = "usage: %prog [options] directory"
    parser = optparse.OptionParser( usage = usage )

    parser.add_option( "-f", "--filename", action="store",
                       dest="Filename", type = "string", default="tuples.lst",
                       help="File to write output tuple filenames to.")

    (options, args) = parser.parse_args()

    if len( args ) != 1:
        print parser.usage
        sys.exit( 1 )

    d = args[ 0 ].rstrip( '/' )

    if not os.path.exists( d ):
        print "Error, directory does not exist"
        sys.exit( 1 )

    if not os.path.isdir( d ):
        print "Error, directory is not a directory"
        sys.exit( 1 )

    sys.exit( main( options.Filename, d ) )
