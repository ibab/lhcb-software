#!/usr/bin/env python

# imports
import sys, optparse, os
# alias for conveniece
import GaudiPython
DecisionMap = GaudiPython.gbl.DecisionMap

def processFile( filename, dm , mismatches ):
    if not os.path.exists(filename):
        print '%s does not exist' %filename
        return
    rootFile = TFile( filename )
    d = rootFile.Get( "TupleHltDecReports" )
    tree = d.Get( "HltDecReportsTuple" )
    branches = tree.GetListOfBranches()
    decisions = set()
    lineName = None
    for branch in branches:
        branchName = branch.GetName()
        if branchName.find( "Decision" ) != -1:
            decisions.add( branchName ) ## was lines
    for decision in decisions:
        lineName = decision
    
    print "processing " + filename

    ientry = 0
    for entry in tree:
        ientry = ientry +1
        runNumber = getattr( entry, "runNumber" )
        eventNumber = getattr( entry, "eventNumber" )
        key = runNumber, eventNumber
        for decision in decisions:
            val = getattr( entry, decision )
            dec = dm.decision( runNumber, eventNumber, decision )
            if val != dec:
                print "Mismatch! " + str( runNumber ) + " " + str( eventNumber )+ " " + decision + " " + ",single:" + str(val) + ",all:" +str(dec)
                print './write_single_event.py --outfile=/tmp/mvesteri/%s_%s_%s.dst --events=%s' %(runNumber,eventNumber,decision,ientry-1)
                if not key in mismatches:
                    mismatches[ key ] = [ decision ]
                else:
                    mismatches[ key ] += [ decision ]

def main( filename ):

    tuples = dict()
    try:
        tupleList = open( filename )
        for line in tupleList:
            line = line.strip()
            if 0 == len( line ) or line[ 0 ] == '#': continue
            t = line.split()
            tuples[ t[ 0 ] ] = t[ 1 ]
    finally:
        tupleList.close()

    if not 'allLines' in tuples.keys():
        print 'Error, no root file listed for the allLines job.'
        return 2

    rootFile = TFile( tuples[ 'allLines' ] )
    d = rootFile.Get( "TupleHltDecReports" )
    tree = d.Get( "HltDecReportsTuple" )
    branches = tree.GetListOfBranches()
    decisions = set()
    for branch in branches:
        branchName = branch.GetName()
        if branchName.find( "Decision" ) != -1:
            decisions.add( branchName )
        
    dm = DecisionMap()

    entries = tree.GetEntries()
    for i in xrange( entries ):
        tree.GetEntry( i )
        if i % 1000 == 0:print str( i )
        runNumber = getattr( tree, "runNumber" )
        eventNumber = getattr( tree, "eventNumber" )
        for decision in decisions:
            val = getattr( tree, decision )
            dm.addDecision( runNumber, eventNumber, decision, val )

    mismatches = dict()
    tuples.pop( 'allLines' )
    for decision, filename in tuples.items():
        processFile( filename, dm, mismatches )
        
    print "\n"
    print "Found mismatches: "
    for key, decisions in mismatches.items():
        s = decisions[ 0 ]
        for dec in decisions[ 1 : ]:
            s += "," + dec
        print str( key[ 0 ] ) + " " + str( key[ 1 ] ) + " " + s
        
    return 0

if __name__ == "__main__":

    usage = "usage: %prog filename"
    parser = optparse.OptionParser( usage = usage )
    
    (options, args) = parser.parse_args()

    if len( args ) != 1:
        print parser.usage
        sys.exit( 1 )

    filename = args[ 0 ]
    if not os.path.exists( filename ):
        print "Error, input file containing the tuples does not exist"
        sys.exit( 1 )

    from ROOT import ( TFile, TTree )

    sys.exit( main( filename ) )
