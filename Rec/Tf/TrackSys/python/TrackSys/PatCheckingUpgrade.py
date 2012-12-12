from Gaudi.Configuration import GaudiSequencer
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

def PatCheckingUpgrade():
   ### match hits and tracks
   log.warning("No truth checking implemented yet.")
   from Configurables import UnpackMCParticle, UnpackMCVertex, PrLHCbID2MCParticle
   truthSeq = GaudiSequencer("RecoTruthSeq")
   #truthSeq.Members = [UnpackMCParticle(), UnpackMCVertex(), PrLHCbID2MCParticle()]

   
   from Configurables import PrTrackAssociator
   #GaudiSequencer("MCLinksTrSeq").Members = [PrTrackAssociator()]
   
   from Configurables import PrChecker, PrDebugTrackingLosses
   #GaudiSequencer("CheckPatSeq").Members += [PrChecker()]
   #GaudiSequencer("CheckPatSeq").Members += [PrDebugTrackingLosses()]
