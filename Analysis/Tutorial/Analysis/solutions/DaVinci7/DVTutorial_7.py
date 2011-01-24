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
from DecayTreeTuple.Configuration import *
tuple = DecayTreeTuple("TutorialTree")
tuple.Decay = "[B_s0 -> (^J/psi(1S) => ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-)]cc"
tuple.InputLocations = [ "Bs2JpsiPhi" ]
tuple.ToolList +=  [
    "TupleToolMCTruth",
    "TupleToolMCBackgroundInfo",
    "TupleToolEventInfo",
]

# Use a LoKi::Hybrid::TupleTool
LoKi_All=tuple.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_All")

LoKi_All.Variables =  {
      "P" : "P",
      "PT" : "PT"
}
tuple.addTool(LoKiTuple)

#Configure the TupleToolTutorial

TutTool=tuple.addTupleTool("TupleToolTutorial")
TutTool.absCharge=True #false by default

# Configure the MCTruth information

MCTruth=tuple.addTupleTool("TupleToolMCTruth")
MCTruth.ToolList =  [
    "MCTupleToolKinematic",
    "MCTupleToolHierarchy" ]

# Configure some branches

tuple.addBranches=({
    "B" :  "[B_s0]cc : [B_s0 -> (J/psi(1S) => mu+ mu-) (phi(1020) -> K+ K-)]cc",
    "KPlus" :  "[B_s0]cc -> (phi(1020) -> ^K+ K-) ? ",
    "KMinus" :  "[B_s0]cc -> (phi(1020) -> K+ ^K-) ? "
    })

LoKi_B=tuple.B.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_B")

LoKi_B.Variables =  {
    "PID" : "ID"
    , "BPVDIRA" : "BPVDIRA"
    , "MassDiff_Bs0" : "DMASS('B_s0')" 
    , "VSep" : "MINTREE(ABSID=='J/psi(1S)',VFASPF(VZ))-MINTREE(ABSID=='phi(1020)',VFASPF(VZ))"
    }

def mySharedConf(branch):
    atool=branch.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_K")
    atool.Variables =  {
        "PID" : "ID"
        ,  "P2" : "P*P"
        ,  "TrChi2" : "TRCHI2DOF"
        }
    
mySharedConf(tuple.KPlus)
mySharedConf(tuple.KMinus)

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
