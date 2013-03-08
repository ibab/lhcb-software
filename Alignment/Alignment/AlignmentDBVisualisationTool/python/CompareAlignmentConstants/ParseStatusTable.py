"""
Module for parsing the status table (eg. Alignment/AlignmentDBVisualisationTool/data/LHCbStatus2012.txt)
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "November 2012"

__all__ = ( "StatusTimePeriod"
          , "statusPeriodsFromTable"
          )

import re, os

from GaudiDetSvcTools import parseTimeMin, parseTimeMax

class StatusTimePeriod(object):
    """
    Time period representation (for status period list)
    """

    class Type(object):
        """
        Type. Name and basic properties
        """
        def __init__(self, name, needAlignment):
            self.name = name
            self.needAlignment = needAlignment
        def __str__(self):
            return self.name

    MagDown = Type("MagDown", True )
    MagUp   = Type("MagUp"  , True )
    MagOff  = Type("MagOff" , True )
    TS      = Type("TS"     , False)
    MD      = Type("MD"     , False)
    VDM     = Type("VDM"    , False)

    def __init__(self, status, startTime, endTime):
        self.status = self._parseStatus(status)
        self.startTime = startTime
        self.endTime   = endTime
        assert self.startTime < self.endTime
    def _parseStatus(self, status):
        tokens = [ tok.strip("()") for tok in status.split() ]
        if len(tokens) > 1 and tokens[1] == "VDM":
            return StatusTimePeriod.VDM
        else:
            return getattr(StatusTimePeriod, tokens[0])
    def __str__(self):
        return "%s from %s to %s" % ( self.status, self.startTime, self.endTime )

def statusPeriodsFromTable(fileName, nToSkipAtBegin=2):
    """
    Extract the status periods from textfile
    """
    recordregex = re.compile("^(?P<status>[a-zA-Z\(\) ]+)\s+(?P<startDate>[0-9\-]+)\s+(?P<endDate>[0-9\-]+)\s+(.*)$")

    statusPeriods = []
    nSkipped = 0
    for line in open(os.path.expandvars(fileName)):
        if not line.startswith("#"):
            if nSkipped < nToSkipAtBegin:
                nSkipped += 1
            else:
                m = recordregex.match(line)
                if m is not None:
                    startDate = parseTimeMin(m.group("startDate"))
                    endDate = parseTimeMax(m.group("endDate"))
                    period = StatusTimePeriod(m.group("status"), startDate, endDate)
                    statusPeriods.append(period)

    return statusPeriods

def prepareTimePeriods( connection, since, until, tag ):
    """
    Method to prepare the appropriate list of StatusTimePeriods depending on whether the connection is to a local sqlite file or the conditions database
    """
    pSince = parseTimeMin(since)
    pUntil = parseTimeMax(until)
    if len(connection) == 0:
        timePeriods = list( period for period in statusPeriodsFromTable("$ALIGNMENTDBVISUALISATIONTOOLROOT/data/LHCbStatus2011.txt")
                           + statusPeriodsFromTable("$ALIGNMENTDBVISUALISATIONTOOLROOT/data/LHCbStatus2012.txt") if period.startTime < pUntil and period.endTime > pSince )
        for p in timePeriods:
            if p.startTime < pSince:
                logging.debug( "Setting start time of %s to %s" % ( p, pSince ) )
                p.startTime = pSince
            if p.endTime > pUntil:
                logging.debug( "Setting end   time of %s to %s" % ( p, pUntil ) )
                p.endTime = pUntil
    else:
        timePeriods = [ StatusTimePeriod( "MagDown", pSince, pUntil ) ]
    return timePeriods
