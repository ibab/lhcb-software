########################################################################
#
# Options for exercise 8
#
# @author Patrick Koppenburg
# @date 2010-06-07
#
########################################################################

from Gaudi.Configuration import *

line = 'BetaSJpsi2MuMuDetachedLine'
location = '/Event/Dimuon/Phys/'+line+'/Particles'

MessageSvc().Format = "% F%80W%S%7W%R%T %0W%M"
from Configurables import DaVinci
from DecayTreeTuple.Configuration import *

# get classes to build the SelectionSequence
from PhysSelPython.Wrappers import AutomaticData, Selection, SelectionSequence
# Get the Candidates from the DST. AutomaticData is for data on the DST
JpsiSel = AutomaticData(Location = location)
# Filter the Candidate.  Let's throw away everything above 4 GeV
from Configurables import FilterDesktop
_jpsiFilter = FilterDesktop('jpsiFilter', Code = '(M>2500*MeV) & (M<4000*MeV)')

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
# tuple.addTupleTool( "TupleToolGeometry") // already default
# tuple.addTupleTool( "TupleToolKinematic")// already default
tuple.addTupleTool( "TupleToolPropertime")
tuple.addTupleTool( "TupleToolPrimaries")
# tuple.addTupleTool( "TupleToolEventInfo")// already default
tuple.addTupleTool( "TupleToolTrackInfo")
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"
tuple.Inputs = [ JpsiSeq.outputLocation() ]
tuple.addTupleTool(TupleToolTISTOS)
tuple.TupleToolTISTOS.TriggerList = [ "Hlt2DiMuonJPsiDecision", "Hlt2DiMuonJPsiHighPTDecision", "Hlt2DiMuonDetachedJPsiDecision" ]
tuple.TupleToolTISTOS.VerboseHlt2 = True
DaVinci().appendToMainSequence( [ JpsiSeq.sequence(), tuple ] ) 
##################

######### Debugging
# decision filter test
from Configurables import LoKi__HDRFilter   as StripFilter
DaVinci().appendToMainSequence( [ StripFilter( 'StripPassFilter', Code="HLT_PASS('Stripping"+line+"Decision')", Location="/Event/Strip/Phys/DecReports" ) ])

# TES explorer
# from Configurables import StoreExplorerAlg
# DaVinci().appendToMainSequence( [ StoreExplorerAlg(PrintEvt=100) ] ) 

# Print decay tree
# from Configurables import PrintDecayTree
# DaVinci().appendToMainSequence( [ PrintDecayTree( 'PrintDiMuons', Inputs = [ location ] ) ] )

DaVinci().DataType = "2012"
DaVinci().EvtMax = 100000
DaVinci().PrintFreq = 100
DaVinci().TupleFile = "Tutorial8.root"

""" To gerenate the plot. Needs Patrick's private macroes
TTree* T = _file0->Get("Jpsi_Tuple/DecayTree")
T->Show(0)
T->Draw("J_psi_1S_MM")
SandB(T,0,"J_psi_1S_MM","J_psi_1S_Hlt2DiMuonJPsiDecision_TOS","1","J_psi_1S_MM<3500 && J_psi_1S_PT>1000 && J_psi_1S_MM>2500","","Psi_Tos")
TH1D hh("hh","MuMu Mass",50,3020,3170)
T->Draw("J_psi_1S_MM >> hh", "1")
TH1D hh2("hh2","MuMu Mass",50,3020,3170)
T->Draw("J_psi_1S_MM >> hh2", "J_psi_1S_Hlt2DiMuonJPsiDecision_TOS")
TH1D hh3("hh3","MuMu Mass",50,3020,3170)
T->Draw("J_psi_1S_MM >> hh3", "J_psi_1S_Hlt2DiMuonDetachedJPsiDecision_TOS")
RooPlot* r = NiceRooPlot(hh3,hh,"gp")  // only PK has this...
SetTitle("m_{#mu#mu} [MeV/c^{2}]","candidates",r)
png("J/psi detached TOS")
"""

