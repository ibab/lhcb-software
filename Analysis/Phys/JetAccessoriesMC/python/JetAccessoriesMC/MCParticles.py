
# =============================================================================
__author__  = " Albert Bursche "
__date__    = " 2011-07-22 " 
__version__ = "0.1"
# =============================================================================

# =============================================================================
"""
Select MC particles having a chance to pass the offline criteria. Needs updating if these citeria change.
"""

from Configurables import FilterDesktop
MCPartFilter = FilterDesktop("FilteredStableParticles")#,OutputLevel = DEBUG)
MCPartFilter.InputLocations = ["Phys/StdHepMCParticles/Particles"]
MCPartFilter.Code = "(PT > 150*MeV) & (ETA >0)" # corresponds to an 200 MeV cut on reco tracks  
                                                # eta cut for speed optimisation
