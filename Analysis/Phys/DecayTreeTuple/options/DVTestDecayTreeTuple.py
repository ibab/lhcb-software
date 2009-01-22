 ## @file HltDecayTreeTuple.opts
 #
 #  Generic Tuple configuration
 #  @warning Will not work without decay and input data
 #  @code
 #  #importOptions( $HLTSELCHECKERROOT/options/HltDecayTreeTuple.py)
 #  HltDecayTreeTuple.PhysDesktop.InputLocations = ["HltSelBd2MuMuKstar"]
 #  HltDecayTreeTuple.Decay = "[B0 -> (^J/psi(1S) => ^mu+ ^mu-) (^K*(892)0 -> ^K+ ^pi-)]cc"
 #  // optional additional tools fro head or other branches
 #  HltDecayTreeTuple.Branches = {
 #    "Head" : "[B0]cc : [B0 -> (J/psi(1S) => mu+ mu-) (K*(892)0 -> K+ pi-)]cc" 
 #  };
 #  HltDecayTreeTuple.Head.ToolList += [ "TupleToolP2VV" ]
 #  @endcode
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
 #

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import *
########################################################################
#
# If you want to import .opts options, do this first
#
importOptions("$DAVINCIROOT/options/PreloadUnits.opts")
#######################################################################
#
# Selection
#
importOptions( "$B2DILEPTONROOT/options/DoDC06SelBu2eeK.opts" )
sel = GaudiSequencer("SeqPreselBu2LLK")
from Configurables import PrintHeader
PrintHeader("PrintPreselBu2LLK").OutputLevel = 4
########################################################################
#
# The Decay Tuple
#
from Configurables import DecayTreeTuple, PhysDesktop 
tuple = DecayTreeTuple("Tuple")
tuple.ToolList +=  [
      "TupleToolTrigger"
    , "TupleToolMCTruth"
    , "TupleToolMCBackgroundInfo"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPropertime"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
    , "TupleToolTISTOS"
     ]
tuple.addTool( PhysDesktop())
tuple.PhysDesktop.InputLocations = ["DC06SelBu2eeK"]
tuple.Decay = "[B+ -> (^J/psi(1S) => ^e+ ^e-) ^K+]cc"
#tuple.OutputLevel = 1 ;
########################################################################
#
# The Event Tuple
#
from Configurables import EventTuple, TupleToolTrigger
evtTuple = EventTuple()
evtTuple.ToolList = [ "TupleToolTrigger", "TupleToolEventInfo" , "TupleToolGeneration" ]
evtTuple.addTool(TupleToolTrigger())
evtTuple.TupleToolTrigger.VerboseHlt1 = True 
evtTuple.TupleToolTrigger.VerboseHlt2 = True 
########################################################################
#
# The MC truth Tuple
#
from Configurables import MCDecayTreeTuple
mcTuple = MCDecayTreeTuple("MCTuple")
mcTuple.Decay = "[B+ -> ^e+ ^e- ^K+ {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}]cc"
mcTuple.ToolList = [ "MCTupleToolMCTruth", "TupleToolEventInfo"  ]
#mcTuple.OutputLevel = 1
########################################################################
#
# DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = 1000
DaVinci().SkipEvents = 0
DaVinci().DataType = "DC06" # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().HistogramFile = "DVHistos_1.root" # Histogram file
DaVinci().TupleFile = "DecayTreeTuple.root"  # Ntuple
DaVinci().UserAlgorithms = [ sel ]
DaVinci().MoniSequence = [ tuple, evtTuple, mcTuple ]
DaVinci().ReplaceL0BanksWithEmulated = True
DaVinci().HltType = "Hlt1+Hlt2"
DaVinci().Input = [
    "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bu2Kee-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
