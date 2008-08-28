### @file
#
#  Hlt2 Hidden Valley selection
#
#  @author M. Kucharczyk Marcin.Kucharczyk@cern.ch
#  @date 2008-08-21
#
##
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DeterministicPrescaler
from Configurables import PatPV3D, PVOfflineTool, Hlt2HidValley, PhysDesktop
###########################################################################
# Define the sequencer. It must be a member of Hlt2Selections.
# There must be a Prescale algorithm defined as well. By default
# it kill do anything.
#
Hlt2SelHidValleyFilterSequence = GaudiSequencer("Hlt2SelHidValleyFilterSequence")
GaudiSequencer("Hlt2Selections").Members += [ Hlt2SelHidValleyFilterSequence ]
Hlt2SelHidValleyFilterSequence.Members += [ DeterministicPrescaler("PrescaleHlt2SelHidValley") ]
###########################################################################
# Final selection
#
Hlt2PatPV3D = PatPV3D("Hlt2PatPV3D")
Hlt2SelHidValley = Hlt2HidValley("Hlt2SelHidValley")
GaudiSequencer("Hlt2SelHidValleyFilterSequence").Members += [Hlt2PatPV3D, Hlt2SelHidValley]
Hlt2PatPV3D.addTool(PVOfflineTool())
Hlt2PatPV3D.PVOfflineTool.InputTracks = ["Hlt/Track/Velo"]

# DisplacedOff vertices
Hlt2PatPV3D.PVOfflineTool.PVFitterName = "LSAdaptPV3DFitter"
Hlt2PatPV3D.PVOfflineTool.PVSeedingName = "PVSeed3DTool"
Hlt2PatPV3D.OutputVerticesName = "Rec/Vertices/displacedOffVertices"

Hlt2SelHidValley.addTool(PhysDesktop())
Hlt2SelHidValley.PhysDesktop.InputLocations = ["Phys/Hlt2Pions"]
#

