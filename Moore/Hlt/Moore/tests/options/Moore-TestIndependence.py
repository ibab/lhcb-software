#!/usr/bin/env python
#
# Script to test independence of Hlt lines

import os, sys, subprocess, re, optparse

# Configuration
import Gaudi.Configuration
from Configurables import GaudiSequencer as Sequence
from Configurables import EventSelector, HltConf

from Moore.Configuration import Moore

# GaudiPython
from GaudiPython import AppMgr

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
                            return line
                        line = acceptIter.next()
                except StopIteration:
                    return None
        except StopIteration:
            return None
    return None

def main():

    # Setup the option parser
    usage = "usage: %prog [options] inputfile <inputfile>"
    parser = optparse.OptionParser( usage = usage )
    parser.add_option( "-d", "--datatype", action="store", dest="DataType", 
                       default="2009", help="DataType to run on.")
    parser.add_option( "-n", "--evtmax", type="int", action = "store", dest = "EvtMax",
                       default = 10000, help = "Number of events to run" )
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

    # Parse the arguments
    (options, args) = parser.parse_args()

    # Make sure there is data to run on
    if not len( args ):
        print "error, no input data specified"
        return 2

    # Put the options into the Moore configurable
    Moore().ThresholdSettings = options.ThresholdSettings

    Moore().Verbose = options.Verbose
    Moore().EvtMax = options.EvtMax

    Moore().UseDBSnapshot = options.UseDBSnapshot
    Moore().DDDBtag   = options.DDDBtag
    Moore().CondDBtag = options.CondDBtag
    Moore().Simulation = options.Simulation
    Moore().DataType   = options.DataType
    Moore().inputFiles = args

    EventSelector().PrintFreq = 100

    # Instanciate the AppMgr to get the Hlt lines from their Sequences
    appMgr = AppMgr()

    hlt1Seq = Sequence( "Hlt1" )
    hlt1Lines = set()
    for m in hlt1Seq.Members:
        hlt1Lines.add( m.name() )

    hlt2Seq = Sequence( "Hlt2" )
    hlt2Lines = set()
    for m in hlt2Seq.Members:
        hlt2Lines.add( m.name() )

    # The AppMgr is no longer needed
    appMgr.exit()

    # Run the full set of lines
    # Setup the command to run all lines
    runjob = os.environ['MOOREROOT'] + '/tests/options/Moore-RunJob.py'
    cmdList = [ runjob,
                "-n", str( Moore().EvtMax ),
                "--settings", Moore().ThresholdSettings,
                "-d", Moore().DataType,
                "--dddbtag", Moore().DDDBtag,
                "--conddbtag", Moore().CondDBtag ]
    if ( Moore().UseDBSnapshot ):
        cmdList += [ "--dbsnapshot" ]
    if ( Moore().Simulation ):
        cmdList += [ "--simulation" ]
    if ( Moore().Verbose ):
        cmdList += [ "-v" ]
    if len( hlt1Lines ):
        cmdList += [ "--hlt1lines" ]
        lines = list( hlt1Lines )
        string = str( lines[ 0 ] )
        for line in lines[ 1: ]:
            string += "," + line
        cmdList += [ string ]
    if len( hlt2Lines ):
        cmdList += [ "--hlt2lines" ]
        lines = list( hlt2Lines )
        string = str( lines[ 0 ] )
        for line in lines[ 1: ]:
            string += "," + line
        cmdList += [ string ]
    cmdList += args

    if options.Verbose:
        cmdStr = cmdList[ 0 ]
        for cmd in cmdList[ 1 : ]: cmdStr += " " + cmd
        print "Running All Lines:" + cmdStr
    else:
        print "Running All Lines."
    # Spawn the job to run with all lines enabled
    p = subprocess.Popen( cmdList, stdout = subprocess.PIPE, stderr = subprocess.STDOUT )
    o = p.communicate()[ 0 ]
    if ( p.returncode ):
        print "failed running all lines"
        return p.returncode

    # Get the output as a list of lines
    output = []
    for line in o.split( "\n" ):
        if len( line.strip() ):
            output.append( line )

    acceptAllLines = {}

    # Hlt{1,2}Global always runs, so it can be removed here
    hlt1Lines.remove( "Hlt1Global" )
    hlt2Lines.remove( "Hlt2Global" )

    # Find and save all accept lines
    for hltLine in ( hlt1Lines | hlt2Lines ):
        line = find_accept( hltLine, output )
        if line: acceptAllLines[ hltLine ] = line
                            
    # Run each line individually
    # Setup the command to run individual lines
    cmdList = [ runjob,
                "-n", str( Moore().EvtMax ),
                "--settings", Moore().ThresholdSettings,
                "-d", Moore().DataType,
                "--dddbtag", Moore().DDDBtag,
                "--conddbtag", Moore().CondDBtag ]
    if ( Moore().UseDBSnapshot ):
        cmdList += [ "--dbsnapshot" ]
    if ( Moore().Simulation ):
        cmdList += [ "--simulation" ]
    if ( Moore().Verbose ):
        cmdList += [ "-v" ]

    acceptSingleLine = {}

    # Run Hlt1 lines individually
    for hltLine in hlt1Lines:
        lst = cmdList + [ "--hlt1lines", hltLine ]
        lst += args
        if options.Verbose:
            cmdStr = lst[ 0 ]
            for arg in lst[ 1 : ]: cmdStr += " " + arg
            print "Running Hlt1 Line: " + hltLine + " using: " + cmdStr
        else:
            print "Running Hlt1 Line: " + hltLine
        # Spawn the job with a single hlt1 line running
        p = subprocess.Popen( lst, stdout = subprocess.PIPE, stderr = subprocess.STDOUT )
        o = p.communicate()[ 0 ]
        if p.returncode:
            print "failed running job"
            return p.returncode
        output = []
        for line in o.split( "\n" ):
            if len( line.strip() ):
                output.append( line )
        line = find_accept( hltLine, output )
        if line: acceptSingleLine[ hltLine ] = line

    # Hlt2 needs Hlt1 to run, so run all Hlt1 lines when testing Hlt2
    if len( hlt1Lines ):
        cmdList += [ "--hlt1lines" ]
        lines = list( hlt1Lines )
        string = str( lines[ 0 ] )
        for line in lines[ 1: ]:
            string += "," + line
        cmdList += [ string ]

    # Run Hlt2 lines individually
    for hltLine in hlt2Lines:
        lst = cmdList + [ "--hlt2lines", hltLine ]
        lst += args
        ## cmdStr = lst[ 0 ]
        ## for arg in lst[ 1 : ]: cmdStr += " " + arg
        print "Running Hlt2 Line: " + hltLine
        # Spawn the job to run the individual Hlt2 lines
        p = subprocess.Popen( lst, stdout = subprocess.PIPE, stderr = subprocess.STDOUT )
        o = p.communicate()[ 0 ]
        if p.returncode:
            print "failed running job"
            return p.returncode
        output = []
        for line in o.split( "\n" ):
            if len( line.strip() ):
                output.append( line )
        line = find_accept( hltLine, output )
        if line: acceptSingleLine[ hltLine ] = line

    # Save any lines which contain mismatches
    badLines = dict()
    
    # Check for accept line mismatches for all Hlt lines
    for key in acceptAllLines.keys():
        if key in acceptSingleLine.keys():
            allAccept = acceptAllLines[ key ]
            singleAccept = acceptSingleLine[ key ]
            if allAccept != singleAccept:
                badLines[ key ] = { "All" : allAccept,
                                    "Single" : singleAccept }

    # Print all lines with mismatches
    if len( badLines ):
        print "!!! Lines with mismatches !!!\n"
        for key in badLines.keys():
            allAccept = acceptAllLines[ key ]
            singleAccept = acceptSingleLine[ key ]
            print key
            print "All:    " + allAccept
            print "Single: " + singleAccept + "\n"
        return 2
    else:
        print "No mismatches."
        return 0

if __name__ == "__main__":
    sys.exit( main() )
