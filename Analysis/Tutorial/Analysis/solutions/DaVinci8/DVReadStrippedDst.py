########################################################################
#
# Options for exercise 8
#
# @author Patrick Koppenburg
# @date 2010-06-07
#
########################################################################

from Gaudi.Configuration import *

line = 'DiMuonIncLine'
location = '/Event/Dimuon/Phys/'+line 

MessageSvc().Format = "% F%80W%S%7W%R%T %0W%M"
from Configurables import DaVinci

# get classes to build the SelectionSequence
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
# Get the Candidates from the DST.
# Treat particles already on the DST as data-on-demand, but give the full path.
JpsiSel = AutomaticData(Location = location)
# Filter the Candidate. Replace 'ALL' by some cuts.
from Configurables import FilterDesktop
_jpsiFilter = FilterDesktop('jpsiFilter', Code = 'MM<4000')

# make a Selection
JpsiFilterSel = Selection(name = 'JpsiFilterSel',
                          Algorithm = _jpsiFilter,
                          RequiredSelections = [ JpsiSel ])

# build the SelectionSequence
JpsiSeq = SelectionSequence('SeqJpsi',
                             TopSelection = JpsiFilterSel,
                            )

############# DecayTreeTuple
from Configurables import DecayTreeTuple, TupleToolTrigger, TupleToolDecay, TupleToolTISTOS
tuple = DecayTreeTuple("Jpsi_Tuple")
tuple.ToolList +=  [
    "TupleToolGeometry"
    , "TupleToolKinematic"
    , "TupleToolPrimaries"
    , "TupleToolEventInfo"
    , "TupleToolTrackInfo"
    , "TupleToolTISTOS"
    , "TupleToolAngles"
    , "TupleToolPid"
    , "TupleToolPropertime"
    ]
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"
tuple.InputLocations = [ JpsiSeq.outputLocation() ]
tuple.addTool(TupleToolTISTOS)
tuple.TupleToolTISTOS.TriggerList = [ "Hlt2DiMuonUnbiasedJPsiDecision" ]
tuple.TupleToolTISTOS.VerboseHlt2 = True
##################

DaVinci().appendToMainSequence( [ JpsiSeq.sequence(), tuple ] ) 
EventSelector().Input   = [
"   DATAFILE='root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2010/DIMUON.DST/00007956/0000/00007956_00000294_1.dimuon.dst?svcClass=lhcbmdst' TYP='POOL_ROOTTREE' OPT='READ'"]

DaVinci().DataType = "2010"
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 100
DaVinci().TupleFile = "Jpsi.root"

"""
TTree* T = _file0->Get("Jpsi_Tuple/DecayTree")
T->Show(0)
T->Draw("J_psi_1S_MM")
SandB(T,0,"J_psi_1S_MM","J_psi_1SHlt2DiMuonUnbiasedJPsiDecision_TOS","1","J_psi_1S_MM<3500","","Psi_Tos")
TH1D hh("hh","MuMu Mass",100,2900,3400)
T->Draw("J_psi_1S_MM >> hh")
NiceRooPlot(hh,"gp")

"""

