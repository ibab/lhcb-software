##############################################################################
# File for running Velo alignment on FEST data
#
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/AlignVeloHalves.py $ESCHEROPTS/FEST-Data.py
#
# or
#
#   $ESCHEROPTS/gaudiiter.py -n NUMITER -e NUMEVENTS $ESCHEROPTS/AlignVeloHalves.py $ESCHEROPTS/FEST-Data.py
#
##############################################################################

from Configurables import ( Escher, TrackSys, RecSysConf )

from GaudiKernel.ProcessJobOptions import importOptions
importOptions('$STDOPTS/PreloadUnits.opts')

#Escher().DataType  = "2008"
#Escher().DDDBtag   = "default"
#Escher().CondDBtag = "default"
#Escher().Detectors = ["Velo","OT"]
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 100
TrackSys().ExpertTracking += [ "kalmanSmoother" ]
TrackSys().TrackExtraInfoAlgorithms = ['']
RecSysConf().RecoSequence = ["VELO","TT","IT","OT","Tr","Vertex"]

from Configurables import TAlignment
from TAlignment.Alignables import *

elements = Alignables()
elements.VeloLeft("TxTyTzRzRxRy")
constraints = []
#elements.VeloRight("TxTyTzRz")
#constraints = ["Tz","Tx","Ty","Rz"]

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

# Load the velo alignment slce
#from Configurables import ( CondDB, CondDBAccessSvc )
#veloCond = CondDBAccessSvc( 'VeloAlignCond' )
#veloCond.ConnectionString = 'sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/VeloSliceFestNothing.db/LHCBCOND'
#CondDB().addLayer( veloCond )

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED_Data.py
##############################################################################



