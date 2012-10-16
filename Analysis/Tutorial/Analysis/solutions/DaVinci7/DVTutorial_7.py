########################################################################
#
# Options for exercise 7
#
# @author RLambert
# @date 2008-06-03
#
########################################################################
from Gaudi.Configuration import *
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#######################################################################
#
# Load the sequencer from Ex 4 and catch it
#
from DaVinci4.solutions.Bs2JpsiPhi import SeqBs2JpsiPhi
seq = SeqBs2JpsiPhi.sequence()
#######################################################################
#
# DecayTreeTuple
#
from DecayTreeTuple.Configuration import *
tuple = DecayTreeTuple("TutorialTree")
tuple.Decay = "[B_s0 -> (^J/psi(1S) => ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-)]cc"
tuple.Inputs = [  SeqBs2JpsiPhi.outputLocation()]
tuple.ToolList +=  [
#    "TupleToolMCTruth",
#    "TupleToolMCBackgroundInfo",
    "TupleToolEventInfo",
]

# Use a LoKi::Hybrid::TupleTool
LoKi_All=tuple.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_All")

LoKi_All.Variables =  {
      "P" : "P",
      "PT" : "PT"
}

#Configure the TupleToolTutorial

TutTool=tuple.addTupleTool("TupleToolTutorial")
TutTool.absCharge=True #false by default

# Configure the MCTruth information

#MCTruth=tuple.addTupleTool("TupleToolMCTruth")
#MCTruth.ToolList =  [
#    "MCTupleToolKinematic",
#    "MCTupleToolHierarchy" ]

# Configure some branches

tuple.Branches = {
    "B" :  "[B_s0]cc : [B_s0 -> (J/psi(1S) => mu+ mu-) (phi(1020) -> K+ K-)]cc",
    "KPlus" :  "[B_s0]cc -> (phi(1020) -> ^K+ K-) ? ",
    "KMinus" :  "[B_s0]cc -> (phi(1020) -> K+ ^K-) ? "
    }
tuple.addTool(TupleToolDecay("B"))
tuple.addTool(TupleToolDecay("KPlus"))
tuple.addTool(TupleToolDecay("KMinus"))
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
DaVinci().HistogramFile='DVHistos7.root'
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "2012"                    # Default is "MC09"
#
# Add our own stuff
#
DaVinci().UserAlgorithms = [ seq, tuple]
#
#
########################################################################
#
# To run in shell :
# gaudirun.py solutions/DaVinci7/DVTutorial_7.py options/Bs2JpsiPhi2008.py
#
# ... but it's best to run it inside of Ganga
########################################################################
