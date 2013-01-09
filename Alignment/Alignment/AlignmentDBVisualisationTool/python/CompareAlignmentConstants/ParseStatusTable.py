"""
Module for parsing the status table (eg. Alignment/Escher/scripts/DBVisualisationTool/LHCbStatus2012.txt)
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
