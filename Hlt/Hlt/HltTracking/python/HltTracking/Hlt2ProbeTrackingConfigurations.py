## Tag and probe tracking
from HltTracking.Hlt2ProbeTracking import Hlt2ProbeTracking

def Hlt2MuonTTTracking():
     return Hlt2ProbeTracking('Hlt2MuonTTTracking', ProbeTrack = 'MuonTT')

def Hlt2VeloMuonTracking():
    return Hlt2ProbeTracking('Hlt2VeloMuonTracking', ProbeTrack = 'VeloMuon')

def Hlt2FullDownstreamTracking():
    return Hlt2ProbeTracking('Hlt2FullDownstreamTracking', ProbeTrack = 'FullDownstream')
