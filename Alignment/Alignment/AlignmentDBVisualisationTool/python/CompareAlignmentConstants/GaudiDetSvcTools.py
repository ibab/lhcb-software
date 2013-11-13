"""
Methods to extract positions and rotations from the detector service
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August-November 2012"

__all__  = ( "AlignmentsWithIOVs"
           , "extractAlignmentParameters"
           , "TrackingDetectorTESNodes"
           , "TrackingAlignmentCondDBNodes"
           , "getGlobalPositionFromGeometryInfo"
           , "parseTimeMin"
           , "parseTimeMax"
           )

import logging
from datetime import datetime, timedelta
import pytz

from PyCool import cool
from CondDBUI.Admin import timeToValKey

from GaudiPython import gbl
from separatethread import RunInSeparateThread

from SimpleAlignmentRepresentation import Transformation, AlignableTreeNode, loopTrees


############################################################
#                detSvc -> transformations                 #
############################################################

def getGlobalPositionFromGeometryInfo( geo ):
    """
    Extract the global position from geometry info
    (transformation from nominal to db in global frame)
    """
    trans = gbl.vector('double')(3)
    rot = gbl.vector('double')(3)
    gbl.DetDesc.getZYXTransformParameters( geo.toGlobalMatrix(), trans, rot )
    return Transformation( ( trans[0], trans[1], trans[2] ), ( rot[0], rot[1], rot[2] ) )

def getAlignableTreeFromDetectorElement( detElm, nodeValue=lambda det : None, parentName=None ):
    """
    Create a simple alignment tree from a detector element

    If the nodeValue function returns None for a node and all its descendants,
    it will not be added to the resulting tree.

    Helper for extractAlignmentParameters
    """
    name = detElm.name()
    shortName = name
    if parentName: # if not the root node: subtract the parent name
        shortName = name[len(parentName)+1:]

    alignNode = AlignableTreeNode( shortName, nodeValue(detElm) )
    alignNode.children = filter( None, ( getAlignableTreeFromDetectorElement(childDet, nodeValue, name) for childDet in detElm.childIDetectorElements() ) )
    # we also look through nodes without alignment condition
    # but if there is no node value (no alignment) and all children are None
    # they should not be added (the above statement makes sure it does not get added to the parent then)
    if ( alignNode.value is not None ) or any( cn is not None for cn in alignNode.children ):
#        print "Created alignable tree node for element %s" % name
        logging.debug("Created alignable tree node for element %s" % name)
        return alignNode
    else:
        return None

############################################################
#       conversion between datetime and gaudi time         #
############################################################
# Times are in two formats:
# - Gaudi::Time (or IOV timestamps)
# - python datetime.datetime objects (with timezone information)
# the toTimeStamp and toDateTime methods can be used to convert between them,
# parsing from a string to a datetime can be done with parseTimeMin or parseTimeMax

def toTimeStamp(dt):
    """ Convert a datetime or timedelta object to a timestamp

    In case a timedelta object is passed, it is assumed to be
    the difference with 1970-01-01_00:00:00.000UTC.

    The granularity is 1s
    """
    if isinstance(dt, timedelta):
        t = dt
    else:
        t = dt - pytz.utc.localize(datetime(1970, 1, 1, 0))
    return (t.days * 60 * 60 * 24 + t.seconds) * 1000000000

def toDateTime(ts):
    """ Convert a timestamp to a datetime object

    The granularity is 1s
    """
    return pytz.utc.localize(datetime(1970, 1, 1, 0)) + timedelta(seconds=ts/1000000000)

_defaultTZ = "CET"
def _parseTime(timeStr, tzName=_defaultTZ):
    # Format YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC] (based on CondDBUI.Admin.timeToValKey)
    tz = None
    if timeStr.endswith("UTC"):
        timeStr = timeStr[:-3]
        tz = pytz.utc
    else:
        tz = pytz.timezone(tzName)

    for fmtStr in ("%Y-%m-%d", "%Y-%m-%d_%H:%M", "%Y-%m-%d_%H:%M:%S.%f"):
        try:
            return tz.localize(datetime.strptime(timeStr, fmtStr)).astimezone(pytz.utc)
        except:
            pass

def parseTimeMin(timeStr, tzName=_defaultTZ):
    """ Parse a "lower bound" time from string to datetime

    If the parsing fails, the minimum allowed time is returned.
    The allowed format is "YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]";
    the timezone to use if UTC is not specified can be passed
    using the 'tzName' keyword argument (CERN local time by default)
    """
    parsed = _parseTime(timeStr, tzName=tzName)
    return parsed if parsed is not None else toDateTime(cool.ValidityKeyMin)

def parseTimeMax(timeStr, tzName=_defaultTZ):
    """ Parse an "upper bound" time from string to datetime

    If the parsing fails, the maximum allowed time is returned.
    The allowed format is "YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]";
    the timezone to use if UTC is not specified, can be passed
    using the 'tzName' keyword argument (CERN local time by default)
    """
    parsed = _parseTime(timeStr, tzName=tzName)
    return parsed if parsed is not None else toDateTime(cool.ValidityKeyMax)


def combinedIOVs(listOfIOVLists):
    """
    Combine IOVs for different elements (split until every combination has its own IOV)
    """
    if len(listOfIOVLists) == 0:
        return []
    elif ( len(listOfIOVLists) == 1 ) or all( len(iovList) == 0 for iovList in listOfIOVLists ):
        return listOfIOVLists[0]
    else:
        # check that they span the same range
        assert all( iovList[0][0] == listOfIOVLists[0][0][0] for iovList in listOfIOVLists )
        assert all( iovList[-1][1] == listOfIOVLists[0][-1][1] for iovList in listOfIOVLists )
        resList = []
        indices = list( 0 for l in listOfIOVLists )
        begin = listOfIOVLists[0][0][0]
        while not all( idx+1 == len(iovList) for idx, iovList in zip(indices, listOfIOVLists) ):
            endI, endIdx = min( enumerate(indices), key=lambda (i, idx) : listOfIOVLists[i][idx][1] )
            end = listOfIOVLists[endI][endIdx][1]
            assert end == listOfIOVLists[endI][endIdx+1][0]
            if end != begin:
                resList.append( (begin, end) )
            indices[endI] += 1
            begin = end
        resList.append( (begin, listOfIOVLists[0][-1][1]) )
        return resList


############################################################
#               Interaction with the detSvc                #
############################################################

TrackingDetectorTESNodes = { # Detector TES locations
      "Velo" : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
    , "TT"   : "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
    , "IT"   : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
    , "OT"   : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
    }

TrackingAlignmentCondDBNodes = { # short names for DB nodes
      "Velo" : [ "/Conditions/Velo/Alignment/Detectors.xml", "/Conditions/Velo/Alignment/Global.xml", "/Conditions/Velo/Alignment/Modules.xml" ]
    , "TT"   : [ "/Conditions/TT/Alignment/Detectors.xml", "/Conditions/TT/Alignment/Modules.xml", "/Conditions/TT/Alignment/Sensors.xml" ]
    , "IT"   : [ "/Conditions/IT/Alignment/Detectors.xml" ]
    , "OT"   : [ "/Conditions/OT/Alignment/Elements.xml" ]
    }

@RunInSeparateThread # in case the method is called more than once...
def extractAlignmentParameters(
          elementsWithTESAndCondDBNodes
        , since
        , until
        , valueExtractor = lambda detElm : getGlobalPositionFromGeometryInfo(detElm.geometry())
        , DDDBtag        = "default"
        , CondDBtag      = "default"
        , alignDBs       = []
        ):
    """
    The method talking to the detector svc

    Extract from all DetectorElements down from each element in elementsWithTESAndCondDBNodes
    ( format { elm : ( detTES, [ condDBNode ] ) } ), alignment parameters using valueExtractor,
    for all iovs between since and until (datetimes), using the CondDBNodes.
    The default database is configured with database tags DDDBtag and CondDBtag,
    and all alignDBs [ (tag, connectString) ] are added as layers to the CondDB.

    Returns a dict { element : [ ( (iovBegin, iovEnd), alignmentTree ) ] }
    """
    # Static configuration of the application manager
    from Configurables import LHCbApp, ApplicationMgr
    from LHCbKernel.Configuration import FATAL, ERROR, WARNING, INFO, DEBUG, VERBOSE

    ApplicationMgr().AppName     = "AlignmentCollector"
    ApplicationMgr().OutputLevel = ERROR

    LHCbApp().DDDBtag   = DDDBtag
    LHCbApp().CondDBtag = CondDBtag

    # >>> This part stolen from Det/DetCond/tests/scripts/getIOVs.py
    ApplicationMgr().TopAlg      = ["LoadDDDB"]

    from Configurables import EventClockSvc, FakeEventTime

    ecs = EventClockSvc(InitialTime=toTimeStamp(datetime(2010,1,1,12, tzinfo=pytz.utc)))
    ecs.addTool(FakeEventTime, "EventTimeDecoder")
    ecs.EventTimeDecoder.StartTime = ecs.InitialTime
    ecs.EventTimeDecoder.TimeStep = toTimeStamp(timedelta(days=1))
    # <<< + "lhcbcond" below

    layers = [ "LHCBCOND" ]

    if len(alignDBs) > 0:
        from Configurables import CondDB, CondDBAccessSvc
        for i, (connectString, tag) in enumerate(alignDBs):
            layerName = "AlignCond%i" % i
            alignCond = CondDBAccessSvc(layerName)
            alignCond.ConnectionString = connectString
            alignCond.DefaultTAG = tag
            CondDB().addLayer(alignCond)
            layers.append(layerName)

    # run a gaudi application
    from GaudiPython import AppMgr, gbl
    gaudi = AppMgr()
    gaudi.createSvc("UpdateManagerSvc")
    updateManagerSvc = gaudi.service("UpdateManagerSvc", interface="IUpdateManagerSvc")
    gaudi.initialize()

    conddbReaders = list( gaudi.service(name, gbl.ICondDBReader) for name in reversed(layers) )

    detDataSvc = updateManagerSvc.detDataSvc()

    alignmentTrees = dict( ( detName, [] ) for detName in elementsWithTESAndCondDBNodes.iterkeys() )

    for detName, (detPath, condNodes) in elementsWithTESAndCondDBNodes.iteritems():
        ### get the IOVs for all elements, and combine them
        timeLine = [ ( "gap", (since, until) ) ]
        for layerReader in conddbReaders:
            timeLineUpdated = list(timeLine)
            alreadyInserted = 0
            for i, ( typ, (gapBegin, gapEnd) ) in enumerate(timeLine):
                if typ == "gap":
                    iovs = combinedIOVs( [ list( ( max(toDateTime(iov.since.ns()), gapBegin)
                                                 , min(toDateTime(iov.until.ns()), gapEnd  ) )
                                                 for iov in layerReader.getIOVs(
                                                                        node
                                                                      , gbl.ICondDBReader.IOV( gbl.Gaudi.Time(toTimeStamp(gapBegin)), gbl.Gaudi.Time(toTimeStamp(gapEnd)) )
                                                                      , 0 ) )
                                           for node in TrackingAlignmentCondDBNodes[detName] ] )
                    if len(iovs) != 0:
                        updatedTimeSlice = list( ( "iov", (begin, end) ) for begin, end in iovs )
                        if updatedTimeSlice[0][1][0] > gapBegin:
                            updatedTimeSlice.insert(0, ("gap", (gapBegin, updatedTimeSlice[0][1][0])))
                        if updatedTimeSlice[-1][1][1] < gapEnd:
                            updatedTimeSlice.append(("gap", (updatedTimeSlice[-1][1][1], gapEnd)))
                        timeLineUpdated[i+alreadyInserted:i+alreadyInserted+1] = updatedTimeSlice
                        alreadyInserted += len(updatedTimeSlice)-1
            logging.debug("timeline after adding %s : %s" % ( layerReader, timeLine ))
            timeLine = timeLineUpdated
        iovs = list( timespan for typ, timespan in timeLine if typ == "iov" )

        ### For every IOV, extract the parameters
        for begin, end in iovs:
            detDataSvc.setEventTime(gbl.Gaudi.Time(toTimeStamp(begin + (end-begin)/2)))
            updateManagerSvc.newEvent()

            logging.info( "Extracting parameters for %s between %s and %s" % ( detName, begin, end ) )
            detTree = getAlignableTreeFromDetectorElement(
                          gaudi.detSvc().getObject(detPath)
                        , nodeValue  = valueExtractor
                        , parentName = detPath
                        )
            detTree.name = detName

            alignmentTrees[detName].append( ( (begin, end), detTree ) )

    gaudi.finalize()

    gaudi.exit()

    return alignmentTrees

class AlignmentsWithIOVs(object):
    """
    Class that manages a set of alignments with IOVs

    Initialized with a connect string and detector root element names.
    Extracts the parameters and provides access to them
    """
    def __init__(self, connectStringsAndTags, detectorNames, since, until, timezone=_defaultTZ, defaultTag="default", valueExtractor=lambda detElm : getGlobalPositionFromGeometryInfo(detElm.geometry()) if detElm.geometry().alignmentCondition() else None ):
        self.connectStringsAndTags = connectStringsAndTags
        self.detectorNames = detectorNames
        self.since = parseTimeMin(since, tzName=timezone)
        self.until = parseTimeMax(until, tzName=timezone)
        self.defaultTag = defaultTag
        self.valueExtractor = valueExtractor
        self._dets = dict()
        allAlignsForAllDets = extractAlignmentParameters( dict( (det, (TrackingDetectorTESNodes[det], TrackingAlignmentCondDBNodes[det])) for det in self.detectorNames ), since=self.since, until=self.until, CondDBtag = self.defaultTag, alignDBs = self.connectStringsAndTags, valueExtractor=self.valueExtractor )
        for detName, iovAndAlignTreeList in allAlignsForAllDets.iteritems():
            self._dets[detName] = AlignmentsWithIOVs._DetectorAlignmentWithIOVs(iovAndAlignTreeList)

    def loopWithTimesAndValues(self, detName, elmRegex, timePeriods):
        # delegate to the detector
        for it in self._dets[detName].loopWithTimesAndValues(elmRegex, timePeriods):
            yield it

    class _DetectorAlignmentWithIOVs(object):
        """
        Class managing the alignment parameters with IOVs for one detector
        """
        def __init__(self, iovsAndAlignments):
            self.iovsAndAlignments = iovsAndAlignments

        def loopWithTimesAndValues(self, elmRegex, timePeriods):
            alignmentsToUse = []
            # validate timePeriods (each one of them should be contained in one IOV)
            for timePeriod in timePeriods:
                logging.debug("Time period: %s" % timePeriod)
                if timePeriod.status.needAlignment:
                    goodOnes = filter( lambda iovAndAlign : iovAndAlign[0][0] <= timePeriod.startTime and timePeriod.endTime <= iovAndAlign[0][1], self.iovsAndAlignments )
                    if len(goodOnes) == 1:
                        logging.debug("--> %s to %s" % (goodOnes[0][0][0], goodOnes[0][0][1]))
                        alignmentsToUse.append(goodOnes[0][1])
                    else:
                        logging.error("You have a problem for time period %s (%i found)" % (timePeriod, len(goodOnes)))
                else:
                    logging.debug("--> No need for alignment parameter")
            # the loop
            for name, matrixForAll in loopTrees(alignmentsToUse):
                m = elmRegex.match(name)
                if m:
                    yield name, m, matrixForAll
