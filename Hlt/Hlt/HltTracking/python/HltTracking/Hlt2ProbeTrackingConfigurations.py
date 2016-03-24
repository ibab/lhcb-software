"""Tag and probe tracking."""

from HltTracking.Hlt2ProbeTracking import Hlt2ProbeTracking


def Hlt2MuonTTTracking(_enabled=True):
    return Hlt2ProbeTracking('Hlt2MuonTTTracking',
                             _enabled=_enabled,
                             ProbeTrack='MuonTT')


def Hlt2VeloMuonTracking(_enabled=True):
    return Hlt2ProbeTracking('Hlt2VeloMuonTracking',
                             _enabled=_enabled,
                             ProbeTrack='VeloMuon')


def Hlt2FullDownstreamTracking(_enabled=True):
    return Hlt2ProbeTracking('Hlt2FullDownstreamTracking',
                             _enabled=_enabled,
                             ProbeTrack='FullDownstream')
