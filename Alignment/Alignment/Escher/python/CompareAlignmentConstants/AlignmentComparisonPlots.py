"""
Visualise the differences between alignments

Extracts the global positions from the detsvc
and saves TGraphs with time trends or just the difference
between different alignments to a ROOT file.

Examples:
#   Time series
>>> myWorkDir = "/home/pieter/cmtuser/alignment2012"
>>> plotAlignmentParametersTimeSeries( "TrendsPerAlignable.root", "sqlite_file:%s/v6.2series.db/LHCBCOND" % myWorkDir, since="2012-03-01", until="2012-10-01", refConnectString="sqlite_file:%s/Alignment_survey.db/LHCBCOND" % myWorkDir )

#   Simple comparison between different alignment outputs
>>> ( "AlignmentsCompared.root", [ "sqlite_file:%s/Alignment_%s.db/LHCBCOND" % ( myWorkDir, fill ) for fill in [ "2875" ] ], ".+/Alignment_(?P<label>[0-9]+)\.db/LHCBCOND$", refConnectString="sqlite_file:%s/Alignment_survey.db/LHCBCOND" % myWorkDir )
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August-September 2012"

__all__  = ( "rootTimeFromGaudiTime"
           , "getAllIOVs"
           , "makeTimeSeriesTGraphX"
           , "writeTrendsPerAlignable"
           , "plotAlignmentParametersComparison"
           , "plotAlignmentParametersTimeSeries"
           )

import logging
logging.basicConfig(level=logging.INFO)

from SimpleAlignmentRepresentation import   loopTrees

from AlignmentXMLDBTools           import ( DetectorAlignmentSliceWithIOV
                                          , TrackingAlignmentCondDBNodes
                                          )
from GaudiDetSvcTools              import ( extractAlignmentParameters
                                          , TrackingDetectorTESNodes
                                          )

from ROOT import TFile, TGraph, TMultiGraph, kCircle, kDot, gROOT, TCanvas, gStyle
import numpy as np

def rootTimeFromGaudiTime( gaudiTime ):
    """
    Trivial conversion from Gaudi time to ROOT time

    Gaudi time : ns since 1970
    ROOT time  :  s since 1995
    """
    return gaudiTime/1e9 - ( (1995-1970)*365.24*24*60*60 )


from CondDBUI.Admin import timeToValKey
from PyCool import cool

def makeTimeSeriesTGraphX(
        iovs
      , sinceBound="2012-01-01"
      , untilBound="2013-01-01" ):
    """
    Generate the x points for a "staircase" TGraph time series, truncating at sinceBound and untilBound

    iovs       : [ ( begin, end ) ]
    sinceBound : earliest time YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]
    untilBound : latest time YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]

    Output is a numpy array of (ROOT) times
    """
    sinceBoundROOT = rootTimeFromGaudiTime( timeToValKey(sinceBound, cool.ValidityKeyMin) )
    untilBoundROOT = rootTimeFromGaudiTime( timeToValKey(untilBound, cool.ValidityKeyMax) )
    validatedIOVs = [ ( min(max(sinceBoundROOT, rootTimeFromGaudiTime(begin)), untilBoundROOT)
                      , min(max(sinceBoundROOT, rootTimeFromGaudiTime(end)), untilBoundROOT) )
                      for begin, end in iovs ]
    xs = []
    for xi, xf in validatedIOVs:
        xs.append(xi)
        xs.append(xf)
    return np.array(xs)

def writeTrendsPerAlignable(
        alignments
      , fileName
      , topDirName   = "trends"
      , x            = None
      , timeSeries   = False
      , saveAll      = False
      , hasReference = False ):
    """
    Save trend graphs for all non-constant parameters in alignments to a TFile

    Parameters:
     - alignments   : sequence of AlignmentTreeNodes
     - fileName     : output file name (updated by this method)
     - topDirName   : top directory (subdirectories following the detector structure are created if not present)
     - x            : x values (can be times, or a list of labels. If not specified a default is used)
     - timeSeries   : if True, the x are interpreted and displayed as times
     - saveAll      : save also the graphs for parameters that do not change between the alignments at hand (False by default)
     - hasReference : set to True if the first AlignmentTreeNode is to be taken as a "reference"
    """

    logging.info("Writing trends with reference settings hasReference=%s" % hasReference)
    gROOT.SetBatch(True)
    gStyle.SetMarkerStyle(kCircle)
    canv = TCanvas("cBatch01", "cBatch01")

    nNonRefAlignments = len(alignments)
    if hasReference:
        nNonRefAlignments -= 1
    xLabels = None
    if x is None: # default for x
        x = np.linspace( 1.0, 1.*nNonRefAlignments, nNonRefAlignments )
    elif not timeSeries and any( isinstance(entry, str) for entry in x ):
        xLabels = [ str(entry) for entry in x ]
        x = np.linspace( 1.0, 1.*nNonRefAlignments, nNonRefAlignments )

    xRef = None
    if timeSeries:
        xRef = np.array([ x[0] ])
    else:
        xRef = np.array([ x[0]-1. ])

    refTransf = None

    outF = TFile(fileName, "update")
    topDir = outF.GetDirectory(topDirName);
    if not topDir:
        topDir = outF.mkdir(topDirName)
    for name, matrixForAll in loopTrees(alignments):

        if hasReference:
            refTransf = matrixForAll[0]
            allTransf = matrixForAll[1:]
        else:
            allTransf = matrixForAll

        # make sure all directories exist
        parent = topDir
        for folder in name.split("/"):
            folderDir = parent.GetDirectory(folder)
            if not folderDir:
                folderDir = parent.mkdir(folder)
            parent = folderDir
        # go to the directory
        outF.cd("/".join((topDirName, name)))

        # get the graphs to be saved
        for ct in [ "Tx", "Ty", "Tz", "Rx", "Ry", "Rz" ]:
            y = None
            if timeSeries: # double
                y = np.array([ getattr( allTransf[int(i)], ct, 0.0 ) for i in np.linspace(0.0, 1.*len(allTransf), 2*len(allTransf), False) ])
            else:          # just the values
                y = np.array([ getattr( transf, ct, 0.0 ) for transf in allTransf ])
            yRef = None
            if hasReference:
                yRef = np.array([ getattr( refTransf, ct, 0.0 ) ])
            if saveAll or ( hasReference and not all( yi == yRef for yi in y ) ) or ( not hasReference and not all( yi == y[0] for yi in y ) ): # only save non-constant
                # construct the graph

                g = None
                if hasReference:
                    if timeSeries:
                        ng = TGraph(len(x), x, y)
                        g  = TMultiGraph()
                        if timeSeries:
                            g.Add(ng, "L")
                        else:
                            g.Add(ng, "P")
                        g.Add( TGraph(1, xRef, yRef), "P" )
                        canv.Clear()
                        g.Draw("A")
                    else:
                        g = TGraph(len(x)+1, np.concatenate((xRef, x)), np.concatenate((yRef, y)))
                else:
                    g = TGraph(len(x), x, y)

                if ct.startswith("T"):
                    g.GetYaxis().SetTitle("%s (mm)" % ct)
                elif ct.startswith("R"):
                    g.GetYaxis().SetTitle("%s (mrad)" % ct)
                if timeSeries: # time display format
                    g.GetXaxis().SetTimeDisplay(True)
                    g.GetXaxis().SetTimeFormat("%d/%m")
                else:
                    if xLabels is not None:
                        beginAxis = 0.5
                        if hasReference:
                            beginAxis = -0.5
                        g.GetXaxis().Set( len(alignments), beginAxis, 1.*nNonRefAlignments+0.5 )
                        for i, label in enumerate(xLabels):
                            g.GetXaxis().SetBinLabel(i+1, label)

                g.Write(ct)

    gROOT.SetBatch(False)

    outF.Close()

def getAllIOVs( listOfIOVLists ):
    """
    Combine a list of IOV lists to make the most division that covers all different intervals

    Input [ [ ( begin, end ) ] ] where [ ( begin, end ) ] is a division in IOVs
    """
    beginTimesSet = set()
    endTimesSet   = set()
    for iovList in listOfIOVLists:
        beginTimesSet |= set( begin for begin, end in iovList )
        endTimesSet   |= set( end for begin, end in iovList )

    beginOrdered = list(sorted(beginTimesSet))
    endOrdered   = list(sorted(endTimesSet))

    allIOVsList = []
    ib, ie = 0, 0
    while ( ib < len(beginOrdered) ) and ( ie < len(endOrdered) ):
        if endOrdered[ie] > beginOrdered[ib]:
            allIOVsList.append(( beginOrdered[ib], endOrdered[ie] ))
            ib += 1
        else:
            ie += 1

    return allIOVsList

def plotAlignmentParametersTimeSeries( outFileName, sliceConnectString, since="2012-01-01", until="2013-01-01", topDirName="trends", refConnectString=None ):
    """
    Write TGraphs for the time evolution of all changing alignment constants during the specified interval to a file

    Example usage:
    plotAlignmentParametersTimeSeries( "TrendsPerAlignable.root", "sqlite_file:%s/v6.2series.db/LHCBCOND" % myWorkDir, since="2012-03-01", until="2012-10-01", refConnectString="sqlite_file:%s/survey.db/LHCBCOND" % myWorkdir )
    """
    # start with a clean file
    outFile = TFile(outFileName, "RECREATE")
    outFile.Close()

    # Query the XML database to know how many time points we need to sample
    alignmentSlice = DetectorAlignmentSliceWithIOV(sliceConnectString)
    iovsPerNode = list( alignmentSlice.getListOfIOVs(node, since=since, until=until) for node in TrackingAlignmentCondDBNodes.iterkeys() )
    fullIOVs = getAllIOVs(iovsPerNode)
    middleTimes = list( (begin+end)/2 for begin, end in fullIOVs )

    allAlignsForAllDets = extractAlignmentParameters( TrackingDetectorTESNodes, times=middleTimes, alignDBs = [ sliceConnectString ] )
    if refConnectString is not None:
        refAlignsForAllDets = extractAlignmentParameters( TrackingDetectorTESNodes, alignDBs = [ refConnectString ] )
        for detName in TrackingDetectorTESNodes.iterkeys():
            allAlignsForAllDets[detName].insert(0, refAlignsForAllDets[detName][0])

    x = makeTimeSeriesTGraphX( fullIOVs, sinceBound=since, untilBound=until )

    for detName in TrackingDetectorTESNodes.iterkeys():
        logging.info( "Saving alignment time trend for %s to %s" % (detName, outFileName) )
        writeTrendsPerAlignable( allAlignsForAllDets[detName], outFileName, x=x, timeSeries=True, topDirName=topDirName, hasReference=( refConnectString is not None ) )

import re
def plotAlignmentParametersComparison( outFileName, dbConnectStrings, binLabels=None, topDirName="trends", refConnectString=None ):
    """
    Write TGraphs with alignment parameters for all specified alignments to a file

    Example usage:
    plotAlignmentParametersComparison( "AlignmentsCompared.root", [ "sqlite_file:%s/Alignment_%s.db/LHCBCOND" % ( myWorkDir, fill ) for fill in [ "2698", "2795", "2806", "2875" ] ], binLabels=".+/Alignment_(?P<label>[0-9]+)\.db/LHCBCOND$", refConnectString="sqlite_file:%s/survey.db/LHCBCOND" % myWorkdir )

    binLabels can either be a list of strings, or a regular expression matching each of the dbConnectStrings - the named group "label" will then be used as bin label
    """
    # start with a clean file
    outFile = TFile(outFileName, "RECREATE")
    outFile.Close()

    # get the bin labels correct : *if* one string, use it to extract the name (group "label")
    if isinstance( binLabels, str ):
        binLabelPattern = re.compile(binLabels)
        binLabels = [ binLabelPattern.match(connStr).group("label") for connStr in dbConnectStrings ]
        logging.info( "Extracted bin labels from connection strings: %s" % binLabels )

    allAlignsForAllDets = [ extractAlignmentParameters( TrackingDetectorTESNodes, alignDBs = [ connString ] ) for connString in dbConnectStrings ]
    if refConnectString is not None:
        refAlignsForAllDets = extractAlignmentParameters( TrackingDetectorTESNodes, alignDBs = [ refConnectString ] )
        allAlignsForAllDets.insert(0, refAlignsForAllDets)
        binLabels.insert(0, "")

    for detName in TrackingDetectorTESNodes.iterkeys():
        logging.info( "Saving alignment comparison for %s to %s" % (detName, outFileName) )
        alignments = [ alignForAllDets[detName][0] for alignForAllDets in allAlignsForAllDets ]

        writeTrendsPerAlignable( alignments, outFileName, x=binLabels, topDirName=topDirName, hasReference=( refConnectString is not None ) )
