"""
Helper class to extract IOV ranges from a conddb slice
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "August 2012"

__all__  = ( "DetectorAlignmentSliceWithIOV"
           , "TrackingAlignmentCondDBNodes"
           )

import logging

TrackingAlignmentCondDBNodes = { # short names for DB nodes
      "OT"            : "/Conditions/OT/Alignment/Elements.xml"
    , "IT"            : "/Conditions/IT/Alignment/Detectors.xml"
    , "TTDetectors"   : "/Conditions/TT/Alignment/Detectors.xml"
    , "TTModules"     : "/Conditions/TT/Alignment/Modules.xml"
    , "TTSensors"     : "/Conditions/TT/Alignment/Sensors.xml"
    , "VeloDetectors" : "/Conditions/Velo/Alignment/Detectors.xml"
    , "VeloGlobal"    : "/Conditions/Velo/Alignment/Global.xml"
    , "VeloModules"   : "/Conditions/Velo/Alignment/Modules.xml"
    }

from PyCool import cool
from CondDBUI import CondDB
from CondDBUI.Admin import timeToValKey

class DetectorAlignmentSliceWithIOV(object):
    """
    Access to an sql database with validity periods
    """
    def __init__( self, connectionString, defaultTag="HEAD" ):
        """
        Constructor

        Example connection string "sqlite_file:/path_to_db_directory/v6.0series.db/LHCBCOND"
        """
        self.connectionString = connectionString
        self.db = CondDB( connectionString, defaultTag=defaultTag )
    def getListOfIOVs( self, detName,  since="2012-01-01", until="2012-12-01" ):
        """
        Extract all alignment validity periods for detName from a SQL database

        detName is one of the keys in TrackingAlignmentCondDBNodes

        Returns a list of tuples ( beginValidity, endValidity )
        """
        logging.info( "Extracting alignments for %s between %s and %s from %s" % ( detName, since, until, self.connectionString) )
        return [ tuple( res[1:3] ) for res in self.db.getPayloadList( TrackingAlignmentCondDBNodes[detName], timeToValKey(since, cool.ValidityKeyMin), timeToValKey(until, cool.ValidityKeyMax) ) ] # strip payload, channel ID and insertion time, and make tuples
