### @file
#
# overwrites tracks with true tracks
#
# @author P. Koppenburg
# @date 2007-07-30
#
from Gaudi.Configuration import *
from Configurables import HltInsertTrackErrParam, ChargedProtoPAlg, MuonID, GaudiSequencer, TESCheck, TrackAssociator
###############
#
# overwrite all locations
#
HltInsertTrackErrParam().InputLocation = "Hlt/Track/Signal"
MuonID("HltMuonID").TrackLocation = "Hlt/Track/Signal"
ChargedProtoPAlg("Hlt2ChargedProtoPAlg").InputTrackLocation = "Hlt/Track/Signal"
###############
#
# redo association
#
GaudiSequencer("SeqHlt2TruthFilter").Members += [ TESCheck("CheckSignalTracks"),
                                                  TrackAssociator("HltSignalTrackAssociator") ]

TESCheck("CheckSignalTracks").Inputs = ["Hlt/Track/Signal"];
TESCheck("CheckSignalTracks").Stop = FALSE
TrackAssociator("HltSignalTrackAssociator").TracksInContainer = "Hlt/Track/Signal"

