#!/usr/bin/env python
#
# Script to test independence of Hlt lines

import os, sys, re, tarfile, tempfile

def find_accept( startLine, lines ):
    """ Function to obtain an accept line from a list of output lines. """
    iter = lines.__iter__()
    while 1:
        try:
            line = iter.next()
            if re.search( "^" + startLine + "\s", line ):
                acceptIter = iter
                try:
                    for i in range( 6 ):
                        if re.search( r"#accept", line ):
                            return line.strip( "\n" )
                        line = acceptIter.next()
                except StopIteration:
                    return None
        except StopIteration:
            return None
    return None
        
def main():

    tmpdir = tempfile.mkdtemp()
    tar = tarfile.open(sys.argv[ 1 ], "r:bz2")
    filenames = tar.getnames()
    logs = set( filenames )
    hlt1Lines = set()
    hlt2Lines = set()
    for log in logs:
        if log.find( "stdout" ) != -1:
            tar.extract( log, tmpdir )
            if log.find( "Hlt1" ) != -1:
                hlt1Lines.add( log.split( '_')[0] )
            elif log.find( "Hlt2" ) != -1:
                hlt2Lines.add( log.split( '_')[0] )
    tar.close()


    f = open( tmpdir + "/allLines_stdout" )
    output = []     
    try:
        for line in f:
            if len( line.strip() ):
                output.append( line )
    finally:
        f.close()

    # Dict to store the lines in the output of the all lines job containing accept
    acceptAllLines = dict()

    # Find and save all accept lines
    for hltLine in ( hlt1Lines | hlt2Lines ):
        line = find_accept( hltLine, output )
        if line: acceptAllLines[ hltLine ] = line

    # Dict to store the lines in the output of the single lines jobs containing accept
    acceptSingleLine = dict()
    for hltLine in ( hlt1Lines | hlt2Lines ):
        f = open( tmpdir + '/' + hltLine + "_stdout" )
        output = []     
        try:
            for line in f:
                if len( line.strip() ):
                    output.append( line )
        finally:
            f.close()
        line = find_accept( hltLine, output )
        if line: acceptSingleLine[ hltLine ] = line

    # Save any lines which contain mismatches
    badLines = set()
  
    # Check for accept line mismatches for all Hlt lines
    for key in acceptSingleLine.keys():
        allAccept = acceptAllLines[ key ]
        singleAccept = acceptSingleLine[ key ]
        if allAccept != singleAccept:
            badLines.add( key )

    # Print all lines with mismatches and write them to a file
    if len( badLines ):
        f = open( "mismatches.log", "w" )
        print "!!! Lines with mismatches !!!\n"
        for key in badLines:
            allAccept = acceptAllLines[ key ]
            singleAccept = acceptSingleLine[ key ]
            print key
            mismatch = "All:    " + allAccept + "\nSingle: " + singleAccept
            print mismatch
            f.write( key + "\n" )
            f.write( mismatch + "\n" )
        f.close()
    else:
        print "No mismatches."

    # Clean up files
    for filename in filenames:
        if filename.find( "stdout" ) != -1:
            os.remove( tmpdir + '/' + filename )
    os.rmdir( tmpdir )

if __name__ == "__main__":
    sys.exit( main() )
