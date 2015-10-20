#!/usr/bin/env python
import sys, os, optparse, shutil

def main( filename, destination ):
    tuples = dict()
    try:
        tuple_list = open( filename )
        for line in tuple_list:
            line = line.strip()
            if 0 == len( line ) or line[ 0 ] == '#': continue
            t = line.split()
            tuples[ t[ 0 ] ] = t[ 1 ]
    finally:
        tuple_list.close()

    copied_tuples = dict()
    for line, tuple_file in tuples.items():
        out_name = line + "_tuples.root"
        out_file = destination + "/" + out_name
        shutil.copy( tuple_file, out_file )
        copied_tuples[ line ] = out_file

    try:
        tuple_list = open( destination + '/copied_' + filename, 'w' )
        for line, location in copied_tuples.iteritems():
            tuple_list.write( line + ' ' + location + '\n' )
    finally:
        tuple_list.close()

    return 0

if __name__ == "__main__":

    usage = "usage: %prog filename destination"
    parser = optparse.OptionParser( usage = usage )
    
    (options, args) = parser.parse_args()

    if len( args ) != 2:
        print parser.usage
        sys.exit( 1 )

    filename = args[ 0 ]
    destination = args[ 1 ].rstrip( '/' )
    if not os.path.exists( filename ):
        print "Error, input file containing the tuples does not exist"
        sys.exit( 1 )

    if not os.path.exists( destination ):
        print "Error, destination does not exist"
        sys.exit( 1 )

    if not os.path.isdir( destination ):
        print "Error, destination is not a directory"
        sys.exit( 1 )

    sys.exit( main( filename, destination ) )

