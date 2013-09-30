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
# now tell if you want MC or not
simulation = False 
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
tuple.Decay = "[B_s0 -> ^(J/psi(1S) -> ^mu+ ^mu-) ^(phi(1020) -> ^K+ ^K-)]CC"
tuple.Inputs = [  SeqBs2JpsiPhi.outputLocation()]
# tuple.addTupleTool( "TupleToolGeometry") // already default
# tuple.addTupleTool( "TupleToolKinematic")// already default
tuple.addTupleTool( "TupleToolPropertime")
tuple.addTupleTool( "TupleToolPrimaries")
# tuple.addTupleTool( "TupleToolEventInfo")// already default
tuple.addTupleTool( "TupleToolTrackInfo")
tuple.addTupleTool( "TupleToolTagging")

if (simulation):
    tuple.addTupleTool( "TupleToolMCTruth")
    tuple.addTupleTool( "TupleToolMCBackgroundInfo")

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

if (simulation):
    MCTruth=tuple.addTupleTool("TupleToolMCTruth")
    MCTruth.addTupleTool("MCTupleToolKinematic")
    MCTruth.addTupleTool("MCTupleToolHierarchy")

# Configure some branches

tuple.Branches = {  # remove all "^" except where needed.
    "B" :  "^([B_s0 -> (J/psi(1S) -> mu+ mu-) (phi(1020) -> K+ K-)]CC)",
    "KPlus" :  "[B_s0 -> (J/psi(1S) -> mu+ mu-) (phi(1020) -> ^K+ K-)]CC",
    "KMinus" :  "[B_s0 -> (J/psi(1S) -> mu+ mu-) (phi(1020) -> K+ ^K-)]CC "
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
# gaudirun.py solutions/DaVinci7/DVTutorial_7.py options/DimuonR14S20.py
#
# ... but it's best to run it inside of Ganga
########################################################################
