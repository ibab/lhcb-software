from Gaudi.Configuration import GaudiSequencer
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

def PrUpgradeChecking():
   ### match hits and tracks
   log.warning("Run upgrade checkers.")
   from Configurables import UnpackMCParticle, UnpackMCVertex, PrLHCbID2MCParticle
   trackAlgs = TrackSys().getProp("TrackPatRecAlgorithms")
   if "Truth" in trackAlgs :
      truthSeq = GaudiSequencer("RecoTruthSeq")
      truthSeq.Members = [UnpackMCParticle(), UnpackMCVertex(), PrLHCbID2MCParticle()]
   else:
      log.warning("no truth specified.")
      GaudiSequencer("MCLinksTrSeq").Members = [ PrLHCbID2MCParticle() ]
   
   from Configurables import PrTrackAssociator
   GaudiSequencer("MCLinksTrSeq").Members += [ PrTrackAssociator() ]
   
   from Configurables import PrChecker, PrDebugTrackingLosses
   GaudiSequencer("CheckPatSeq").Members += [ PrChecker() ]
   GaudiSequencer("CheckPatSeq").Members += [ PrDebugTrackingLosses() ]
