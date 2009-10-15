########################################################################
#
# Options for exercise 7
#
# @author RLambert
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
#######################################################################
#
# Load the sequencer from Ex 4 and catch it
#
importOptions("$ANALYSISROOT/solutions/DaVinci4/TutorialSeq.py")
tutorialseq = GaudiSequencer("TutorialSeq")
#######################################################################
#
# DecayTreeTuple
#
from Configurables import DecayTreeTuple, TupleToolMCTruth, LoKi__Hybrid__TupleTool, TupleToolDecay, TupleToolTutorial
tuple = DecayTreeTuple("TutorialTree")
tuple.InputLocations = [ "Bs2JpsiPhi" ]
tuple.ToolList +=  [
    "TupleToolMCTruth",
    "TupleToolMCBackgroundInfo",
    "TupleToolEventInfo",
    "LoKi::Hybrid::TupleTool/LoKi_All",
    "TupleToolTutorial"
]
tuple.Decay = "[B_s0 -> (^J/psi(1S) => ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-)]cc"

# Use a LoKi::Hybrid::TupleTool

LoKiTuple = LoKi__Hybrid__TupleTool("LoKi_All")
LoKiTuple.Variables =  {
      "P" : "P"                                                                                           
    , "PT" : "PT"
}
tuple.addTool(LoKiTuple)

#Configure the TupleToolTutorial

TutTuple=TupleToolTutorial()
TutTuple.absCharge=True #false by default
tuple.addTool(TutTuple)

# Configure the MCTruth information

MCTruth = TupleToolMCTruth()
MCTruth.ToolList =  [
      "MCTupleToolKinematic"                                                                                           
    , "MCTupleToolHierarchy" ]
tuple.addTool(MCTruth)

# Configure some branches

tuple.Branches={
      "B" :  "[B_s0]cc : [B_s0 -> (J/psi(1S) => mu+ mu-) (phi(1020) -> K+ K-)]cc"            
    ,  "KPlus" :  "[B_s0]cc -> (phi(1020) -> ^K+ K-) ? "                                                                                         
    ,  "KMinus" :  "[B_s0]cc -> (phi(1020) -> K+ ^K-) ? "                                                                            
}
tuple.addTool(TupleToolDecay, name="B")
LoKi_B=LoKi__Hybrid__TupleTool("LoKi_B")
LoKi_B.Variables =  {
       "PID" : "ID"
    , "BPVDIRA" : "BPVDIRA"
    , "MassDiff_Bs0" : "DMASS('B_s0')" 
    , "VSep" : "MINTREE(ABSID=='J/psi(1S)',VFASPF(VZ))-MINTREE(ABSID=='phi(1020)',VFASPF(VZ))"
}

tuple.B.ToolList+=["LoKi::Hybrid::TupleTool/LoKi_B"]
tuple.B.addTool(LoKi_B)

tuple.addTool(TupleToolDecay, name="KPlus")
LoKi_K=LoKi__Hybrid__TupleTool("LoKi_K")
LoKi_K.Variables =  {
       "PID" : "ID"
    ,  "P2" : "P*P"                                                                                           
    ,  "TrChi2" : "TRCHI2DOF"
}

tuple.KPlus.ToolList+=["LoKi::Hybrid::TupleTool/LoKi_K"]
tuple.KPlus.addTool(LoKi_K)

tuple.addTool(TupleToolDecay, name="KMinus")
tuple.KMinus.ToolList+=["LoKi::Hybrid::TupleTool/LoKi_K"]
tuple.KMinus.addTool(LoKi_K)

tuple.OutputLevel=3

#######################################################################
#
# Configure the application
#
from Configurables import DaVinci
DaVinci().TupleFile = "Tutorial7.root"         # Ntuple
DaVinci().HistogramFile='DVHistos.root'
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "MC09"                    # Default is "MC09"
DaVinci().Simulation   = True                  # It's MC
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ tutorialseq, tuple]
DaVinci().MainOptions  = ""                    # None
#
#
DaVinci().Hlt = True
DaVinci().HltThresholdSettings = "Physics_320Vis_300L0_10Hlt1_Aug09"   # some settings. See HltConf for more.
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci7/DVTutorial_7.py options/Bs2JpsiPhi2008.py
#
# ... but it's best to run it inside of Ganga
########################################################################
