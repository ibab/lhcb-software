##############################################################################
# File for running Velo alignment on FEST data
#
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/2008-MC-Files.py
#   gaudirun.py $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/DC06-Files.py
#
#   $ESCHEROPTS/gaudiiter.py -n 3 -e 1000 $ESCHEROPTS/AlignVeloSensors.py $ESCHEROPTS/2008-MC-Files.py
#
##############################################################################

from Configurables import ( Escher, TrackSys, RecSysConf )

from GaudiKernel.ProcessJobOptions import importOptions
importOptions('$STDOPTS/PreloadUnits.opts')

#Escher().DataType  = "2008"
#Escher().DDDBtag   = "default"
#Escher().CondDBtag = "default"
#Escher().Detectors = ["Velo","OT"]

Escher().InputType = "DIGI"
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 1000
TrackSys().ExpertTracking += [ "kalmanSmoother" ]
TrackSys().TrackExtraInfoAlgorithms = ['']
RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr","Vertex"]

from Configurables import TAlignment
from TAlignment.Alignables import *

elements = Alignables()
#elements.VeloRSensors("TxTy")
#elements.VeloPhiSensors("TxTy")
#elements.VeloModules("TzRxRyRz")
#let's do modules only, for now:
elements.VeloModules("TxTyTzRxRyRz")
constraints = ["Tz","Ty","Tx","Rz","Ry","Rz"]

print "aligning elements ", elements

TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
TAlignment().VertexLocation       = "Rec/Vertex/Primary"
TAlignment().Constraints          = constraints
TAlignment().WriteCondSubDetList  = ['Velo']
#TAlignment().UpdateInFinalize = False

# still set up a track selection
from Configurables import (GaudiSequencer,TrackContainerCopy,TrackSelector,TrackMonitor) 
trackFilterSeq = GaudiSequencer("TrackFilterSeq")
alignSelector  = TrackContainerCopy("AlignSelector",
                                    inputLocation = "Rec/Track/Best",
                                    outputLocation = "Rec/Track/AlignTracks")
alignSelector.addTool( TrackSelector("Selector",
                                     MaxChi2Cut = 10,
                                     MinNVeloRHits = 3,
                                     MinNVeloPhiHits = 3) ) 
trackFilterSeq.Members.append( alignSelector )
trackFilterSeq.Members.append( TrackMonitor("AlignTrackMonitor",
                                            TracksInContainer = "Rec/Track/AlignTracks") )

# Load the velo misalignment
from Configurables import ( CondDB, CondDBAccessSvc )
veloCond = CondDBAccessSvc( 'VeloAlignCond' )
veloCond.ConnectionString = 'sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/B2HH_LHCBCOND_Velo-slice_5sigma.db/LHCBCOND'
CondDB().addLayer( veloCond )

# add some data



##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED_Data.py
##############################################################################



