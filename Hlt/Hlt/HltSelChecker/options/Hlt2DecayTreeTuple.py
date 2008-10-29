### @file Hlt2DecayTreeTuple.opts
 #
 #  Generic Tuple configuration
 #  @warning Will not work without decay and input data
 #  @code
 #  importOptions("$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
 #  DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
 #  DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBd2MuMuKstar"]
 #  DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B0 -> (^J/psi(1S) => ^mu+ ^mu-) (^K*(892)0 -> ^K+ ^pi-)]cc"
 #  # special for head
 #  DecayTreeTuple("Hlt2DecayTreeTuple").Branches = {
 #     "Head" : "[B0]cc : [B0 -> (J/psi(1S) => mu+ mu-) (K*(892)0 -> K+ pi-)]cc"
 #    }
 #  DecayTreeTuple("Hlt2DecayTreeTuple").addTool(TupleToolDecay("Head"))
 #  DecayTreeTuple("Hlt2DecayTreeTuple").Head.ToolList += [ "TupleToolP2VV" ]
 #  @endcode
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
 #/
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple
#
Hlt2DecayTreeTuple = DecayTreeTuple("Hlt2DecayTreeTuple")
ApplicationMgr().TopAlg += [ Hlt2DecayTreeTuple ]
Hlt2DecayTreeTuple.Context = "HLT"

Hlt2DecayTreeTuple.ToolList += [
      "TupleToolTrigger"
    , "TupleToolMCTruth"
    , "TupleToolMCBackgroundInfo"
    , "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPropertime"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "LoKi::Hybrid::TupleTool/LoKiTool" ]

#
# event tuple
#
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )
