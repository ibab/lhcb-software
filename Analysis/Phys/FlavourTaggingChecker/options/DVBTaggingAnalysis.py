########################################################################
#
# Options for BTaggingAnalysis algorithm
#
# @author Marco Musy
# @date 2009-02-23
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer

# If you want to import .opts options, do this first
importOptions("$DAVINCIROOT/options/PreloadUnits.opts")

########################################################################
#
# Some selection. 
#
#importOptions("$B2DILEPTONROOT/options/DoPreselBu2LLK.opts")
#preselSeq = GaudiSequencer("SeqPreselBu2LLK")
#importOptions( "$B2DPLUSXROOT/options/DC06SelBs2DsPi.opts")
importOptions( "$DAVINCIROOT/options/DC06SelBs2DsPi.opts")
preselSeq = GaudiSequencer("SeqDC06SelBs2DsPi")


########################################################################
#
# BTaggingAnalysis 
#
importOptions( "$FLAVOURTAGGINGOPTS/BTaggingTool.py" )

from Configurables import BTaggingAnalysis, PhysDesktop, FilterToFixOppositeBFractions, BackgroundCategory, CombinedParticleMaker, ProtoParticleMUONFilter, ProtoParticleCALOFilter, ProtoParticleRICHFilter

tagana = BTaggingAnalysis("BTaggingAnalysis")
pd = PhysDesktop()
pd.ParticleMakerType = "CombinedParticleMaker"
pd.addTool( CombinedParticleMaker() )
pd.CombinedParticleMaker.ExclusiveSelection = True;
pd.CombinedParticleMaker.addTool(ProtoParticleMUONFilter("Muon"))
pd.CombinedParticleMaker.Muon.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'0.0'" ]
pd.CombinedParticleMaker.addTool(ProtoParticleCALOFilter("Electron"))
pd.CombinedParticleMaker.Electron.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'5.0'" ]
pd.CombinedParticleMaker.addTool(ProtoParticleRICHFilter("Kaon"))
pd.CombinedParticleMaker.Kaon.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'8.0' CombDLL(k-p)>'-4.0'"]
##
pd.InputLocations = ["Phys/BTaggingAnalysis", "Phys/DC06SelBs2DsPi" ]
tagana.addTool( pd )
#tagana.OutputLevel = 2


########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 500                        # Number of events
#DaVinci().DataType = "2008"                  # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().HistogramFile = "DVHistos.root"     # Histogram file
DaVinci().TupleFile     = "analysis.root"     # Ntuple
DaVinci().UserAlgorithms = [ preselSeq, tagana ]  # The algorithms

########################################################################
# HLT
DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
#DaVinci().Hlt2IgnoreHlt1Decision = True    ## Hlt2 irrespective of Hlt1
DaVinci().HltType = 'Hlt1'                  ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'

########################################################################
# Data file BsDspi:
EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002034/DST/0000/00002034_00000001_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
