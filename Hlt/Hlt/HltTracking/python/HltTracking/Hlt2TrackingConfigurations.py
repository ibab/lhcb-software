"""Provide the HLT2 tracking configurations.

Note:
    In 2011, this ran only the forward tracking, hence the name.
    For 2012, it has been modified to do the seeding and clone killing,
    but the name has been kept to simplify all the lines which depend on it.
"""

from Hlt2Tracking import Hlt2Tracking
from HltTrackNames import HltDefaultFitSuffix

__author__ = "V. Gligorov vladimir.gligorov@cern.ch"


def Hlt2BiKalmanFittedForwardTracking(_enabled=True):
    """Return the configurable for long tracking with Kalman fit."""
    return Hlt2Tracking("Hlt2LongTracking",
        _enabled         = _enabled,
        FastFitType      = HltDefaultFitSuffix,
        Hlt2Tracks       = 'Long',
        DoSeeding        = True,
        # Set this to the same in both tracking configurations:
        CreateBestTracks = True
        )


def Hlt2BiKalmanFittedDownstreamTracking(_enabled=True):
    """Return configurable for downstream tracking with Kalman fit."""
    return Hlt2Tracking("Hlt2DownstreamTracking",
        _enabled         = _enabled,
        FastFitType      = HltDefaultFitSuffix,
        Hlt2Tracks       = 'Downstream',
        DoSeeding        = True,
        # Set this to the same in both tracking configurations:
        CreateBestTracks = True
        )


def setDataTypeForTracking(trackingInstance, dataType):
    """Set the DataType of the configurable."""
    trackingInstance.setProp("DataType", dataType)
