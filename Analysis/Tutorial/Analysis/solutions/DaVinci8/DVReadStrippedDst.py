########################################################################
#
# Options for exercise 8
#
# @author Patrick Koppenburg
# @date 2010-06-07
#
########################################################################

from Gaudi.Configuration import *

line = 'NeuroBayesMuMuLine'
location = '/Event/Dimuon/Phys/'+line 

MessageSvc().Format = "% F%80W%S%7W%R%T %0W%M"
from Configurables import DaVinci

# get classes to build the SelectionSequence
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
# Get the Candidates from the DST. AutomaticData is for data on the DST
JpsiSel = AutomaticData(Location = location)
# Filter the Candidate.  Let's throw away everything above 4 GeV
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
DaVinci().appendToMainSequence( [ JpsiSeq.sequence(), tuple ] ) 
##################

#from Configurables import StoreExplorerAlg
#from Configurables import LoKi__HDRFilter   as StripFilter
#DaVinci().appendToMainSequence( [ StripFilter( 'StripPassFilter', Code="HLT_PASS('StrippingNeuroBayesMuMuLineDecision')", Location="/Event/Strip/Phys/DecReports" ) ])
# DaVinci().appendToMainSequence( [ StoreExplorerAlg(PrintEvt=100) ] ) 

DaVinci().DataType = "2010"
DaVinci().EvtMax = -1
DaVinci().PrintFreq = 100
DaVinci().TupleFile = "Jpsi.root"

"""
TTree* T = _file0->Get("Jpsi_Tuple/DecayTree")
T->Show(0)
T->Draw("J_psi_1S_MM")
SandB(T,0,"J_psi_1S_MM","J_psi_1SHlt2DiMuonUnbiasedJPsiDecision_TOS","1","J_psi_1S_MM<3500 && J_psi_1S_PT>1000","","Psi_Tos")
TH1D hh("hh","MuMu Mass",100,2900,3400)
T->Draw("J_psi_1S_MM >> hh", "J_psi_1S_PT>1000")
NiceRooPlot(hh,"gp")  // only PK has this...
TH1D hh2("hh2","MuMu Mass",100,3500,3800)
T->Draw("J_psi_1S_MM >> hh2", "J_psi_1S_PT>1000")
NiceRooPlot(hh2,"gp")  // only PK has this...
TH1D hh3("hh3","MuMu Mass",100,2900,4000)
hh3.GetXaxis()->SetTitle("Dimuon mass [MeV]")
T->Draw("J_psi_1S_MM >> hh3", "J_psi_1S_PT>1000")
.L TwoGaussians.C
TwoGaussians(&hh3,3095.,3680.)  // only PK has this...
"""

