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

# this causes problems with the levels set in the shell script
#logging.basicConfig(level=logging.INFO)

import re

from GaudiDetSvcTools              import ( AlignmentsWithIOVs
                                          , parseTimeMin, parseTimeMax
                                          )

from ParseStatusTable              import ( statusPeriodsFromTable
                                          , StatusTimePeriod
                                          , prepareTimePeriods
                                          )

from Pages                         import ( folders
                                          , drawTimeTrends
                                          , drawTimeTrendReference
                                          , drawDiffAlignment
                                          , drawDiffReference
                                          , ElementGroup, ElementGroups
                                          , generateGeometryDict
                                          , drawHeatPlot
                                          )

def plotAlignmentParametersTimeSeries( elmGroup, dofs
                                     , sliceConnectString=None, sliceTag="HEAD"
                                     , since="2012-01-01", until="2013-01-01"
                                     , refConnectString=None, refTag="HEAD"
                                     , outputdir="."
                                     , defaultTag="cond-20130114"
                                     , addStats=False ):
    """
    Plot time trends of alignment sliceConnectString with tag sliceTag (+default layer with given tag)
    for given dofs of detector group elmGroup, optionally adding a reference
    """
    # get the time periods
    pSince = parseTimeMin(since)
    pUntil = parseTimeMax(until)
    timePeriods = list( period for period in statusPeriodsFromTable("$ALIGNMENTDBVISUALISATIONTOOLROOT/data/LHCbStatus2011.txt") + statusPeriodsFromTable("$ALIGNMENTDBVISUALISATIONTOOLROOT/data/LHCbStatus2012.txt") if period.startTime < pUntil and period.endTime > pSince )
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

def plotAlignmentParametersHeat( detector
                               , dofs
                               , inputAlignments
                               , outputDir="."
                               , layers=["all"]
                               , drawNames=False ):
    """
    Plot heat maps of alignment differences for each layer of the specified detector
    """

    # detector dictionary which contains an ElementGroup instantiation to call the database methods appropriately, and a tuple of the detector's layer names
    D = { "TT" : { "elmGroup" : ElementGroups["TT.Sensors"]
                 , "layers"   : tuple( "TT" + str(u[0]) + "/TT" + str(v) + "Layer" for u in ( ("a", ("aX", "aU")), ("b", ("bV", "bX")) ) for v in u[1] )
                 }
        , "IT" : { "elmGroup" : ElementGroup("IT/(?P<page>Station[1-3]/(?:ASide|CSide|Top|Bottom)Box)/Layer(?:X1|U|V|X2)/Ladder[1-7]/Sector/(?P<elm>Sensor[1-2])$", 1, 2)
                              # IT layer list intentionally omits box names
                 , "layers"   : tuple( "Station" + str(u) + "/Layer" + str(v) for u in range(1, 4) for v in ("X1", "U", "V", "X2") )
                 }
        , "OT" : { "elmGroup" : ElementGroups["OT.Modules"]
                 , "layers"   : tuple( "T" + str(u) + "/" + v for u in range(1, 4) for v in ("X1", "U", "V", "X2") )
                 }
        }

    # record the comparison being made, ultimately gets written in the plot titles
    comparisonDescriptionString = "%s   relative to   %s" % tuple( alignment[0] for alignment in inputAlignments )
    print comparisonDescriptionString

    if layers == ['all']:
        logging.debug("Retrieving list of all layers for the %s" % detector)
        layers = D[detector]['layers']
    # check if a valid layer name was given in case the --layer option was used
    if not layers[0] in D[detector]['layers']:
        from sys import exit
        print "%r is not a valid layer name for the %s.\n Use the -h option to see the layer name conventions." % (layers[0], detector)
        exit()

    globalSince = "2010-01-01"
    globalUntil = "2013-01-01"
    name, connection, since, until, tag = inputAlignments[0]
    alignmentOne = AlignmentsWithIOVs( connection, [ detector ], globalSince, globalUntil, tag )
    timePeriodsOne = prepareTimePeriods(connection, since, until, tag)
    name, connection, since, until, tag = inputAlignments[1]
    if len(inputAlignments[0][1]) == 0 and len(connection) == 0:
        # don't bother connecting to the conditions database again if the two alignments are both IOVs (both in the conditions database)
        alignmentTwo = alignmentOne
    else:
        alignmentTwo = AlignmentsWithIOVs( connection, [ detector ], globalSince, globalUntil, tag )
    timePeriodsTwo = prepareTimePeriods(connection, since, until, tag)

    logging.debug("Initializing detector tuple")
    detectorTuple = tuple( ( node, pageName, matrix[0] ) for node, pageName, matrix in alignmentOne.loopWithTimesAndValues( detector, D[detector]['elmGroup'], timePeriodsOne ) )

    logging.debug("Calculating detector tuple")
    for i, ( first, second ) in enumerate( zip( detectorTuple, alignmentTwo.loopWithTimesAndValues( detector, D[detector]['elmGroup'], timePeriodsTwo )) ):
        for dof in dofs:
            # convert rad to mrad
            conversion = 1
            if dof.startswith("R"):
                conversion = 1000
            # make the calculation
            setattr( detectorTuple[i][2], dof, ( getattr( first[2], dof ) - getattr( second[2][0], dof )) * conversion )

    # generate layer fill list by matching detectorTuple element name slices to the layer name
    for layer in layers:
        print "Processing layer %s" % layer
        layerFillList = []
        _elementLayer = lambda cellNumber: nameSplit[1] + "/" + nameSplit[cellNumber]
        if detector in ("OT", "TT"):
            cell = 2
        elif detector == "IT":
            cell = 3 # ignore IT Box names
        logging.debug("Building layer %s fill list" % layer)
        for element in detectorTuple:
            nameSplit = element[0].split("/")
            if _elementLayer(cell) == layer:
                layerFillList.append(element)

        logging.debug("Generating geometry dictionary")
        GeometryDict = generateGeometryDict(detector, layer)

        for dof in dofs:
            # make a color range which is symmetric about zero
            diffMax = max([getattr(i[2], dof) for i in layerFillList])
            diffMin = min([getattr(i[2], dof) for i in layerFillList])
            diffColorRange = max(abs(diffMax),abs(diffMin))
            # make a precision cutoff if necessary
            if dof.startswith("T"):
                # 1 micron for silicon detectors
                precisionCutoff = 0.001
                # 10 micron for straw tubes
                if detector == "OT":
                    precisionCutoff = 0.01
            elif dof.startswith("R"):
                # update this, tentatively set to 1 microradian
                precisionCutoff = 0.001
            if diffColorRange < precisionCutoff:
                diffColorRange = precisionCutoff

            logging.debug("Generating %s plot" % dof)
            drawHeatPlot(comparisonDescriptionString, detector, layer, dof, outputDir, layerFillList, GeometryDict, diffColorRange, drawNames)
