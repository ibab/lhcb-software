###
# Options for running HLT2 only on truth
#
# @author P. Koppenburg
# @date 2007-07-20
#
##
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, FilterTrueTracks, HltCorrelations
#
GaudiSequencer("SeqTrueSignalTracks").Members = [ FilterTrueTracks() ]
FilterTrueTracks.TracksPath = [ "Hlt/Track/Forward" ]
FilterTrueTracks.OutputPath = "Hlt/Track/Signal"
#
# You *MUST* also configure FilterTrueTracks.MCDecayFinder.Decay
#
#
###
# add to correlations
#
HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "FilterTrueTracks" ]
