### @file 
 #
 # Hlt exclusive selections options
 #
 # @author P. Koppenburg
 # @date 2006-02-01
###
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
# ---------------------------------------------------------------------
#  HLT exclusive Selections
# ---------------------------------------------------------------------
Hlt2 = GaudiSequencer("Hlt2", Context = 'HLT' )
#  just in case
if Hlt2 not in GaudiSequencer("Hlt").Members : GaudiSequencer("Hlt").Members += [ Hlt2 ]
# ---------------------------------------------------------------------
#  Make sure Hlt1 is passed
# ---------------------------------------------------------------------
Hlt2.Members += [ GaudiSequencer("Hlt2CheckHlt1Passed") ]
# ---------------------------------------------------------------------
#  Full reconstruction of all tracks 
# ---------------------------------------------------------------------
Hlt2.Members += [ GaudiSequencer("HltRecoSequence") ]
# ---------------------------------------------------------------------
#  ProtoParticles and Particles
# ---------------------------------------------------------------------
importOptions( "$HLTCONFROOT/options/Hlt2Particles.opts" )
# ---------------------------------------------------------------------
#  Empty Sequencer for MC truth filtering --- filled in HltSelChecker
# ---------------------------------------------------------------------
Hlt2.Members += [ GaudiSequencer("SeqHlt2TruthFilter") ] 
importOptions( "$HLTCONFROOT/options/HltTrackAssociator.opts" )
# ---------------------------------------------------------------------
# Initialise Hlt2Shared particles
# ---------------------------------------------------------------------
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Shared.py" )
# ---------------------------------------------------------------------
#  Final selections
# ---------------------------------------------------------------------
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Selections.py" )
# --------------------------------------------------------------------
#  Decision
# --------------------------------------------------------------------
Hlt2.Members += [ "HltSelectionsDecision/Hlt2Decision" ] 

