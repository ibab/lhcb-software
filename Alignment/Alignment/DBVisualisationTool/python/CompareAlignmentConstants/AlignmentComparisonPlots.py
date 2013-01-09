"""
Visualise the differences between alignments

The python methods behind the command-line interface

"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August-November 2012"

__all__  = ( "plotAlignmentParametersComparison"
           , "plotAlignmentParametersTimeSeries"
           )

import logging
logging.basicConfig(level=logging.INFO)

import re

from GaudiDetSvcTools              import ( AlignmentsWithIOVs
                                          , parseTimeMin, parseTimeMax
                                          )

from ParseStatusTable              import ( statusPeriodsFromTable
                                          , StatusTimePeriod
                                          )

from Pages                         import ( folders
                                          , drawTimeTrends
                                          , drawTimeTrendReference
                                          , drawDiffAlignment
                                          , drawDiffReference
                                          )

def plotAlignmentParametersTimeSeries( elmGroup, dofs
                                     , sliceConnectString=None, sliceTag="HEAD"
                                     , since="2012-01-01", until="2013-01-01"
                                     , refConnectString=None, refTag="HEAD"
                                     , outputdir="."
                                     , defaultTag="cond-20121116"
                                     , addStats=False ):
    """
    Plot time trends of alignment sliceConnectString with tag sliceTag (+default layer with given tag)
    for given dofs of detector group elmGroup, optionally adding a reference
    """
    # get the time periods
    pSince = parseTimeMin(since)
    pUntil = parseTimeMax(until)
    timePeriods = list( period for period in statusPeriodsFromTable("$DBVISUALISATIONTOOLROOT/data/LHCbStatus2011.txt") + statusPeriodsFromTable("$DBVISUALISATIONTOOLROOT/data/LHCbStatus2012.txt") if period.startTime < pUntil and period.endTime > pSince )
    for p in timePeriods:
        if p.startTime < pSince:
            logging.debug( "Setting start time of %s to %s" % ( p, pSince ) )
            p.startTime = pSince
        if p.endTime > pUntil:
            logging.debug( "Setting end   time of %s to %s" % ( p, pUntil ) )
            p.endTime = pUntil

    detsToLoad = elmGroup.split(".")[0:1]

    # get the alignments
    alignConnectStringsAndTags = list()
    if sliceConnectString is not None:
        alignConnectStringsAndTags.append((sliceConnectString, sliceTag))
    theAlignmentParameters = AlignmentsWithIOVs( alignConnectStringsAndTags, detsToLoad, since, until, defaultTag=defaultTag )

    drawTimeTrends( timePeriods, elmGroup, theAlignmentParameters, dofs, addStats=addStats )

    if refConnectString:
        refAlignmentParameters = AlignmentsWithIOVs( [ (refConnectString, refTag) ], detsToLoad, since, until, defaultTag=defaultTag )
        drawTimeTrendReference( timePeriods, elmGroup, refAlignmentParameters, dofs )

    for folder in folders.itervalues():
        folder.save(outputdir)

def plotAlignmentParametersComparison( elmGroup, dofs
                                     , sliceConnectStringsAndTags
                                     , binLabels=None
                                     , refConnectString=None, refTag="HEAD"
                                     , outputdir="."
                                     , since="2010-01-01"
                                     , until="2013-01-01"
                                     , defaultTag="default" ):
    """
    Plot alignment differences between the slices (with tags), and with respect to the reference
    """
    pSince = parseTimeMin(since)
    pUntil = parseTimeMax(until)
    timePeriods = [ StatusTimePeriod( "MagDown", pSince, pUntil ) ]

    detsToLoad = elmGroup.split(".")[0:1]

    # get the bin labels correct : *if* one string, use it to extract the name (group "label")
    if isinstance( binLabels, str ):
        binLabelPattern = re.compile(binLabels)
        binLabels = list( binLabelPattern.match(connStr).group("label") for connStr, tag in sliceConnectStringsAndTags )
        logging.debug( "Extracted bin labels from connection strings: %s" % binLabels )
    if not isinstance(binLabels, list):
        binLabels = list( None for cS, t in sliceConnectStringsAndTags )

    for ( connStr, tag ), label in zip(sliceConnectStringsAndTags, binLabels):
        theAlignmentParameters = AlignmentsWithIOVs( [ ( connStr, tag ) ], detsToLoad, since, until, defaultTag=defaultTag )
        drawDiffAlignment( timePeriods, elmGroup, theAlignmentParameters, dofs, label )
    if refConnectString:
        refAlignmentParameters = AlignmentsWithIOVs( [ (refConnectString, refTag) ], detsToLoad, since, until, defaultTag=defaultTag )
        drawDiffReference( timePeriods, elmGroup, refAlignmentParameters, dofs )

    for folder in folders.itervalues():
        folder.save(outputdir)
