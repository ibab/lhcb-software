##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#
#   gaudirun.py $ESCHEROPTS/AlignOTCosmics.py $ESCHEROPTS/2008-Cosmic-Data.py
#
# or
#
#   $ESCHEROPTS/gaudiiter.py -n NUMITER -e NUMEVENTS $ESCHEROPTS/AlignOTCosmics.py $ESCHEROPTS/2008-Cosmic-Data.py
#
##############################################################################

from Configurables import ( Escher, TrackSys, RecSysConf )

from GaudiKernel.ProcessJobOptions import importOptions
importOptions('$STDOPTS/PreloadUnits.opts')

Escher().DataType     = "2008"
Escher().Kalman = True
Escher().Millepede = False
Escher().TrackContainer = "Rec/Track/Best" # Velo, VeloR, Long, Upstream, Downstream, Ttrack, Muon or Calo
Escher().EvtMax = 1000
Escher().SpecialData = [ "fieldOff", "cosmics"]
TrackSys().ExpertTracking += [ "noDrifttimes","kalmanSmoother" ]
TrackSys().TrackPatRecAlgorithms = [ "PatSeed" ]
TrackSys().TrackExtraInfoAlgorithms = ['']

RecSysConf().RecoSequence = ["TT","IT","OT","Tr"]

# load a special database
from Configurables import ( CondDB, CondDBAccessSvc )
otGeom = CondDBAccessSvc( 'OTGeom' )
otGeom.ConnectionString = 'sqlite_file:/afs/cern.ch/user/j/janos/dbase/OTDDDBCroissant.db/DDDB'
CondDB().addLayer( otGeom )
otCond = CondDBAccessSvc( 'OTCond' )
otCond.ConnectionString = 'sqlite_file:/afs/cern.ch/user/j/janos/dbase/LHCBCOND_changes.db/LHCBCOND'
CondDB().addLayer( otCond )

# configure the alignment
from Configurables import TAlignment
from TAlignment.Alignables import *
elements = Alignables()
elements.OTCFramesASide("TxTyTzRz")
elements.OTCFramesCSide("TxTyTzRz")
constraints = [
    "T1X1C : T1X1UCSide : Tx Ty Tz Rz",
    "T1X1A : T1X1UASide : Tx Ty Tz Rz",
    "T3X2C : T3X1UCSide : Tx Ty Tz Rz",
    "T3X2A : T3X1UASide : Tx Ty Tz Rz"
    ]
TAlignment().ElementsToAlign      = list(elements)
TAlignment().TrackLocation        = "Rec/Track/AlignTracks"
TAlignment().Constraints          = constraints
TAlignment().WriteCondSubDetList  = ['OT']

#from Configurables import (PatSeeding, PatSeedingTool)
#PatSeeding = PatSeeding("PatSeeding")
#PatSeeding.addTool( PatSeedingTool("PatSeedingTool") )
#PatSeeding.PatSeedingTool.MaxOTOccupancy         = 0.01
#PatSeeding.PatSeedingTool.MaxITOccupancy         = 0.01
#PatSeeding.PatSeedingTool.CommonXFraction        = 0.
#PatSeeding.PatSeedingTool.MaxUsedFractPerRegion  = 0.
#PatSeeding.PatSeedingTool.MaxUsedFractITOT       = 0.
#PatSeeding.PatSeedingTool.MaxUsedFractLowQual    = 0.

# set up a track selection
from Configurables import (GaudiSequencer,TrackContainerCopy,TrackSelector,TrackMonitor) 
trackFilterSeq = GaudiSequencer("TrackFilterSeq")
alignSelector  = TrackContainerCopy("AlignSelector",
                                    inputLocation = "Rec/Track/Best",
                                    outputLocation = "Rec/Track/AlignTracks")
alignSelector.addTool( TrackSelector("Selector", MaxChi2Cut = 5) )
trackFilterSeq.Members.append( alignSelector )
trackFilterSeq.Members.append( TrackMonitor("AlignTrackMonitor",
                                            TracksInContainer = "Rec/Track/AlignTracks") )

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED_Data.py
##############################################################################



