"""
Methods to extract positions and rotations from the detector service
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August 2012"

__all__  = ( "extractAlignmentParameters"
           , "TrackingDetectorTESNodes"
           , "getAlignableTreeFromDetectorElement"
           , "getGlobalPositionFromGeometryInfo"
           )

import logging

from SimpleAlignmentRepresentation import Transformation, AlignableTreeNode

from GaudiPython import gbl
from separatethread import RunInSeparateThread

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

def getAlignableTreeFromDetectorElement( detElm, nodeFilter=lambda det : True, nodeValue=lambda det : None, parentName=None ):
    """
    Create a simple alignment tree from a detector element

    Helper for extractAlignmentParameters
    """
    name = detElm.name()
    shortName = name
    if parentName: # if not the root node: subtract the parent name
        shortName = name[len(parentName)+1:]

    alignNode = AlignableTreeNode( shortName, nodeValue(detElm) )
    alignNode.children = [ getAlignableTreeFromDetectorElement(childDet, nodeFilter, nodeValue, name) for childDet in detElm.childIDetectorElements() if nodeFilter(childDet) ]
    return alignNode

############################################################
#               Interaction with the detSvc                #
############################################################

TrackingDetectorTESNodes = { # Detector TES locations
      "Velo" : "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
    , "TT"   : "/dd/Structure/LHCb/BeforeMagnetRegion/TT"
    , "IT"   : "/dd/Structure/LHCb/AfterMagnetRegion/T/IT"
    , "OT"   : "/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
    }

@RunInSeparateThread # in case the method is called more than once...
def extractAlignmentParameters(
          elementsWithTES
        , valueExtractor = lambda detElm : getGlobalPositionFromGeometryInfo(detElm.geometry())
        , times          = [ 0 ]
        , DDDBtag        = "default"
        , CondDBtag      = "HEAD"
        , alignDBs       = []
        , filterRegexp   = r'/dd/Structure/LHCb/((BeforeMagnetRegion/(Velo|TT))|(AfterMagnetRegion/T/(IT|OT)))[a-zA-Z0-9]*'
        ):
    """
    The method talking to the detector svc

    Extract from all DetectorElements with names matching filterRegexp,
    down from each element in elementsWithTES, alignment parameters using valueExtractor,
    querying the gaudi detector service at specified times.
    The LHCb Gaudi application is configured with database tags DDDBtag and CondDBtag,
    and all alignDBs are added as layers to the CondDB.

    Returns a dict { element : [ alignmentTrees ] } with the elements in the list corresponding to the times
    """
    # Static configuration of the application manager
    from Configurables import LHCbApp, ApplicationMgr
    from LHCbKernel.Configuration import FATAL, ERROR, WARNING, INFO, DEBUG, VERBOSE

    ApplicationMgr().AppName     = "AlignmentCollector"
    ApplicationMgr().OutputLevel = ERROR

    LHCbApp().DDDBtag   = DDDBtag
    LHCbApp().CondDBtag = CondDBtag

    if len(alignDBs) > 0:
        from Configurables import CondDB, CondDBAccessSvc
        for i, connectString in enumerate(alignDBs):
            alignCond = CondDBAccessSvc( 'AlignCond%i' % i )
            alignCond.ConnectionString = connectString
            CondDB().addLayer( alignCond )

    # run a gaudi application
    from GaudiPython import AppMgr

    gaudi = AppMgr()
    gaudi.createSvc("UpdateManagerSvc")
    updateManagerSvc = gaudi.service("UpdateManagerSvc", interface="IUpdateManagerSvc")
    gaudi.initialize()

    detDataSvc = updateManagerSvc.detDataSvc()

    import re
    allowedPattern = re.compile(filterRegexp)

    alignmentTrees = dict( ( detName, [] ) for detName in elementsWithTES.iterkeys() )
    for timePoint in times:
        gaudiTime = gbl.Gaudi.Time(timePoint)
        detDataSvc.setEventTime(gaudiTime)
        updateManagerSvc.newEvent()

        for detName, detPath in elementsWithTES.iteritems():
            logging.info( "Extracting parameters for %s at %s" % ( detName, getattr(gaudiTime, "format")(True, "%c") ) )
            detTree = getAlignableTreeFromDetectorElement(
                          gaudi.detSvc().getObject(detPath)
                        , nodeFilter = lambda detElm : detElm.geometry().alignmentCondition() and allowedPattern.match(detElm.name())
                        , nodeValue  = valueExtractor
                        , parentName = detPath
                        )
            detTree.name = detName

            alignmentTrees[detName].append(detTree)

    gaudi.finalize()

    gaudi.exit()

    return alignmentTrees
