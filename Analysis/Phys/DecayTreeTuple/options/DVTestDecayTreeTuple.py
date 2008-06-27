## @file HltDecayTreeTuple.opts
 #
 #  Generic Tuple configuration
 #  @warning Will not work without decay and input data
 #  @code
 #  #include "$HLTSELCHECKERROOT/options/HltDecayTreeTuple.opts"
 #  HltDecayTreeTuple.PhysDesktop.InputLocations = {"Phys/HltSelBd2MuMuKstar"};
 #  HltDecayTreeTuple.Decay = "[B0 -> (^J/psi(1S) => ^mu+ ^mu-) (^K*(892)0 -> ^K+ ^pi-)]cc";
 #  // optional additional tools fro head or other branches
 #  HltDecayTreeTuple.Branches = {
 #    "Head" : "[B0]cc : [B0 -> (J/psi(1S) => mu+ mu-) (K*(892)0 -> K+ pi-)]cc" 
 #  };
 #  HltDecayTreeTuple.Head.ToolList += { "TupleToolP2VV" };
 #  @endcode
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 20080-03-07
 #

from Gaudi.Configuration import *
from Configurables import DecayTreeTuple, PhysDesktop, NeuralNetTmva

importOptions( "$B2DILEPTONROOT/options/DVDC06SelBu2eeK.opts" )

# importOptions( "$L0DUOPTS/L0DUFromRaw.opts")
importOptions( "$HLTSYSROOT/options/L0.opts")
importOptions( "$HLTSYSROOT/options/Hlt.opts" )

ApplicationMgr().TopAlg += [ NeuralNetTmva() ] 
importOptions( "$NNTOOLSROOT/options/NeuralNetTmva.opts")

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
    , "TupleToolTrackInfo" ]

tuple.addTool( PhysDesktop())
tuple.PhysDesktop.InputLocations = ["Phys/DC06SelBu2eeK"]
tuple.Decay = "[B+ -> (^J/psi(1S) => ^e+ ^e-) ^K+]cc"

ApplicationMgr().TopAlg += [ tuple ]
ApplicationMgr().EvtMax = 1000 

NTupleSvc().Output = ["FILE1 DATAFILE='Tuple.root' TYP='ROOT' OPT='NEW'"]

