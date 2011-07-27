
# =============================================================================
__author__  = "Albert Bursche"
__date__    = " 2011-07-22 " 
__version__ = "0.1"
# =============================================================================

# =============================================================================
"""
Select a track sample for jet reconstruction. 
"""

from Configurables import FilterDesktop
SelectedTracks =  FilterDesktop("FilteredPions")#,OutputLevel = DEBUG)
SelectedTracks.InputLocations = ["/Event/Phys/StdNoPIDsPions/Particles",
                                 "Phys/StdNoPIDsDownPions/Particles"]
SelectedTracks.Code = "(PT > 200*MeV) & (PERR2/(P*P) < 0.04) "
#SelectedTracks.Preambulo = [ "from LoKiProtoParticles.decorators import *"]

