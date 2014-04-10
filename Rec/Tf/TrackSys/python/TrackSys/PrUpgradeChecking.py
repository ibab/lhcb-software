from Gaudi.Configuration import GaudiSequencer
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

def PrUpgradeChecking():
   ### match hits and tracks
   log.warning("Run upgrade checkers.")
   from Configurables import UnpackMCParticle, UnpackMCVertex, PrLHCbID2MCParticle
   # Check if VP is part of the list of detectors.
   from Configurables import LHCbApp, VPClusterLinker
   withVP = False
   if hasattr(LHCbApp(), "Detectors"):
      if LHCbApp().isPropertySet("Detectors"):
         if 'VP' in LHCbApp().upgradeDetectors(): withVP = True
   trackTypes = TrackSys().getProp("TrackTypes")
   if "Truth" in trackTypes :
      truthSeq = GaudiSequencer("RecoTruthSeq")
      truthSeq.Members = [UnpackMCParticle(), UnpackMCVertex()]
      if withVP: truthSeq.Members += [VPClusterLinker()]
      truthSeq.Members += [PrLHCbID2MCParticle()]
   else:
      if withVP:
        GaudiSequencer("MCLinksTrSeq").Members = [VPClusterLinker(), PrLHCbID2MCParticle()]
      else:
        GaudiSequencer("MCLinksTrSeq").Members = [PrLHCbID2MCParticle()]
   
   from Configurables import PrTrackAssociator
   GaudiSequencer("MCLinksTrSeq").Members += [ PrTrackAssociator() ]
   
   from Configurables import PrChecker, PrDebugTrackingLosses
   GaudiSequencer("CheckPatSeq").Members += [ PrChecker() ]
   GaudiSequencer("CheckPatSeq").Members += [ PrDebugTrackingLosses() ]
