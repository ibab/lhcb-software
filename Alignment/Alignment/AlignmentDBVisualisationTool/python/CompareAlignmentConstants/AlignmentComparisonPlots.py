"""
Visualise the differences between alignments

The python methods behind the command-line interface

"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August-November 2012"

__all__  = ( "plotAlignmentParametersComparison"
           , "plotAlignmentParametersTimeSeries"
           )

import logging, copy

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
                                     , timezone="CET"
                                     , refConnectString=None, refTag="HEAD"
                                     , outputdir="."
                                     , defaultTag="cond-20130114"
                                     , addStats=False ):
    """
    Plot time trends of alignment sliceConnectString with tag sliceTag (+default layer with given tag)
    for given dofs of detector group elmGroup, optionally adding a reference
    """
    # get the time periods
    pSince = parseTimeMin(since, timezone)
    pUntil = parseTimeMax(until, timezone)
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
    theAlignmentParameters = AlignmentsWithIOVs( alignConnectStringsAndTags, detsToLoad, since, until, timezone=timezone, defaultTag=defaultTag )

    drawTimeTrends( timePeriods, elmGroup, theAlignmentParameters, dofs, timezone=timezone, addStats=addStats )

    if refConnectString:
        refAlignmentParameters = AlignmentsWithIOVs( [ (refConnectString, refTag) ], detsToLoad, since, until, timezone=timezone, defaultTag=defaultTag )
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

# detector dictionary which contains an ElementGroup instantiation to call the database methods appropriately, and a tuple of the detector's layer names
Dets = {
    "TT" : { "elmGroup" : ElementGroups["TT.Sensors"]
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

def plotAlignmentParametersHeat( det
                               , dofs
                               , inputAlignments
                               , outputDir="."
                               , layers=["all"]
                               , drawNames=False
                               , AlarmThresholds={"Tx":-1,"Ty":-1,"Tz":-1,"Rx":-1,"Ry":-1,"Rz":-1} # default thresholds of -1 double as 'alarm-mode-off' flags
                               , returnFlag=False
                                 ):
    """
    Plot heat maps of alignment differences for each layer of the specified detector
    """
    
    # record the comparison being made, ultimately gets written in the plot titles
    comparisonDescriptionString = "%s   relative to   %s" % tuple( alignment[0] for alignment in inputAlignments )
    print comparisonDescriptionString

    if layers == ['all']:
        logging.debug("Retrieving list of all layers for the %s" % det)
        layers = Dets[det]['layers']
    # check if a valid layer name was given in case the --layer option was used
    if not layers[0] in Dets[det]['layers']:
        from sys import exit
        print "%r is not a valid layer name for the %s.\n Use the -h option to see the layer name conventions." % (layers[0], det)
        print "exiting..."
        exit()

    name, connection, since, until, tag = inputAlignments[0]
    timePeriodsOne = prepareTimePeriods(connection, since, until, tag)
    if ( 0 == len(connection) and 0 == len(inputAlignments[1][1]) ):
        # both alignments are IOVs: only connect to the database once
        if parseTimeMin(since) <= parseTimeMin(inputAlignments[1][2]):
            globalSince = since
            globalUntil = inputAlignments[1][3]
        else:
            globalSince = inputAlignments[1][2]
            globalUntil = until
        alignmentOne = AlignmentsWithIOVs( connection, [ det ], globalSince, globalUntil, tag )
        alignmentTwo = alignmentOne
    else:
        alignmentOne = AlignmentsWithIOVs( connection, [ det ], since, until, tag )
        name, connection, since, until, tag = inputAlignments[1]
        alignmentTwo = AlignmentsWithIOVs( connection, [ det ], since, until, tag )
    name, connection, since, until, tag = inputAlignments[1]
    timePeriodsTwo = prepareTimePeriods(connection, since, until, tag)

    logging.debug("Initializing detector tuple")
    detectorTuple = tuple( ( node, pageName, matrix[0] ) for node, pageName, matrix in alignmentOne.loopWithTimesAndValues( det, Dets[det]['elmGroup'], timePeriodsOne ) )
    if ( 0 == len(detectorTuple) ):
        from sys import exit
        print "Invalid time period. Check if an IOV entered is actually a technical stop, etc."
        print "exiting..."
        exit()

    logging.debug("Calculating detector tuple")
    for i, ( first, second ) in enumerate( zip( detectorTuple, alignmentTwo.loopWithTimesAndValues( det, Dets[det]['elmGroup'], timePeriodsTwo )) ):
        for dof in dofs:
            parameters = [ getattr( first[2], dof ), getattr( second[2][0], dof ) ]
            if dof.startswith("R"):
                from math import pi
                for j, parameter in enumerate( parameters ):
                    # scrub off 2pi (though the parameter is usually epsilon +/- pi)
                    if   (  3*pi / 2 < parameter ):
                        logging.debug("-> removing 2pi from  %s..." % parameter)
                        parameter = parameter - 2*pi
                        logging.debug("         ...it's now  %s" % parameter)
                    elif ( -3*pi / 2 > parameter ):
                        logging.debug("-> removing -2pi from %s..." % parameter)
                        parameter = 2*pi + parameter
                        logging.debug("          ...it's now %s" % parameter)
                    # scrub off pi
                    if   (  pi / 2 < parameter ):
                        logging.debug("-> removing pi from  %s..." % parameter)
                        parameter = parameter - pi
                        logging.debug("        ...it's now  %s" % parameter)
                    elif ( -pi / 2 > parameter ):
                        logging.debug("-> removing -pi from %s..." % parameter)
                        parameter = parameter + pi
                        logging.debug("         ...it's now %s" % parameter)
                    parameters[j] = parameter * 1000
            difference = parameters[0] - parameters[1]
            logging.debug("diff is %s" % difference)
            setattr( detectorTuple[i][2], dof, difference )

    # initialize dictionary of max alignment differectes to return
    MaxDiffs = {}
    
    # generate layer fill list by matching detectorTuple element name slices to the layer name
    for layer in layers:
        print "Processing layer %s" % layer
        layerFillList = []
        _elementLayer = lambda cellNumber: nameSplit[1] + "/" + nameSplit[cellNumber]
        if det in ("OT", "TT"):
            cell = 2
        elif det == "IT":
            cell = 3 # ignore IT Box names
        logging.debug("Building layer %s fill list" % layer)
        for element in detectorTuple:
            nameSplit = element[0].split("/")
            if _elementLayer(cell) == layer:
                layerFillList.append(element)

        logging.debug("Generating geometry dictionary")
        GeometryDict = generateGeometryDict(det, layer)

        for dof in dofs:
            # make a color range which is symmetric about zero
            diffsList = [ getattr(i[2], dof) for i in layerFillList ]
            diffMax = max( diffsList )
            diffMin = min( diffsList )
            diffRange = max(abs(diffMax),abs(diffMin))
            # copy this to return to scanner
            if dof not in MaxDiffs:
                MaxDiffs[dof] = {}
            logging.debug("diffMax is %s and diffMin is %s" % ( diffMax, diffMin ))
            if ( abs(diffMax) > abs(diffMin) ):
                MaxDiffs[dof][layer] = copy.copy(diffMax)
            else:
                MaxDiffs[dof][layer] = copy.copy(diffMin)
#            MaxDiffs[dof][layer] = copy.copy(diffRange)
            # set units and precision cutoff
            if dof.startswith("T"):
                units = "(mm)"
                # 1 micron for silicon detectors
                precisionCutoff = 0.001
                # 10 micron for straw tubes
                if det == "OT":
                     precisionCutoff = 0.01
            elif dof.startswith("R"):
                units = "(mrad)"
                # update this, tentatively set to 1 microradian
                precisionCutoff = 0.001
            if diffRange < precisionCutoff:
                diffRange = precisionCutoff
            alarm = AlarmThresholds[dof]

            # -1 means alarm mode is off
            if alarm == -1:
                logging.debug("Generating %s plot" % dof)
                drawHeatPlot(comparisonDescriptionString, det, layer, dof, outputDir, layerFillList, GeometryDict, diffRange, drawNames)
            # otherwise alarm mode is on
            elif not alarm == -1:
                if alarm <= precisionCutoff:
                    print " * Alarm threshold for %s set below precision cutoff of %s " % (dof, precisionCutoff) + units + ",\n thus the alarm threshold will always be exceeded. *"
                if diffRange < alarm:
                    print " * Alarm threshold for %s not exceeded, absolute maximum of alignment differences is %s " % (dof, diffRange) + units + "\n   Skipping plot. *"
                    pass
                elif diffRange >= alarm:
                    alarmString = "* Alarm mode: threshold of %s " % (alarm) + units + " exceeded *\n"
                    print " " + alarmString.rstrip("\n")
                    print " * Absolute maximum of %s aligment differences is %s " % (dof, diffRange) + units + " *"
                    logging.debug("Generating %s plot" % dof)
                    drawHeatPlot(alarmString + comparisonDescriptionString, det, layer, dof, outputDir, layerFillList, GeometryDict, diffRange, drawNames)

    if returnFlag:
        return MaxDiffs
