################################################################################
# First build the selection
################################################################################
from Configurables import CombineParticles

_jpsi2mumu = CombineParticles("Jpsi2MuMu")
_jpsi2mumu.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
_jpsi2mumu.CombinationCut = "ADAMASS('J/psi(1S)')<30*MeV"
_jpsi2mumu.MotherCut = "(VFASPF(VCHI2/VDOF)<10)"

from PhysSelPython.Wrappers import Selection
from StandardParticles import StdLooseMuons, StdLooseKaons
LooseJpsi2MuMu = Selection("SelLooseJpsi2MuMu",
                           Algorithm = _jpsi2mumu,
                           RequiredSelections = [StdLooseMuons])

from Configurables import FilterDesktop
_jpsifilter = FilterDesktop("_jpsiFilter",
                            Code = "(PT>1*GeV) & (P>3*GeV)")
Jpsi2MuMu = Selection("SelJpsi2MuMu",
                      Algorithm = _jpsifilter,
                      RequiredSelections = [LooseJpsi2MuMu])

_phi2kk = CombineParticles("Phi2KK")
_phi2kk.DecayDescriptor = "phi(1020) -> K+ K-"
_phi2kk.CombinationCut = "ADAMASS('phi(1020)')<50*MeV"
#_phi2kk.MotherCut = "(VFASPF(VCHI2/VDOF)<100)"

#####################################################################################
### use a BDT to select the phi
##################################################################################### 

# BDT wheight file
wheightfile = "/afs/cern.ch/user/s/sneubert/cmtuserDev/DaVinci_HEAD/Phys/MVADictTools/options/TestPhi2KK.xml"

# input variables needed by BDT
Vars = {
     "lab1_PT"               : "PT",
     "lab1_IPCHI2_OWNPV"     : "MIPCHI2DV(PRIMARY)",
     "lab2_PT"               : "CHILD(PT,1)",
     "lab3_PT"               : "CHILD(PT,2)",
     "lab2_IPCHI2_OWNPV"     : "CHILD(MIPCHI2DV(PRIMARY),1)",
     "lab3_IPCHI2_OWNPV"     : "CHILD(MIPCHI2DV(PRIMARY),2)",
    }

# use the VALUE functor to access IParticleValue tools 
_phi2kk.MotherCut = "VALUE('LoKi__Hybrid__DictValue/MVAResponse')> -0.5"

# add the complete tool chain with one command:
from MVADictHelpers import *
addTMVAclassifierValue(Component=_phi2kk,XMLFile=wheightfile,Variables=Vars,Name="MVAResponse")


#####################################################################################
#####################################################################################

# Finish the selection
Phi2KK = Selection("SelPhi2KK",
                   Algorithm = _phi2kk,
                   RequiredSelections = [StdLooseKaons])

_bs2JpsiPhi = CombineParticles("Bs2JpsiPhi",
                               DecayDescriptor = "B_s0 -> phi(1020) J/psi(1S)",
                               CombinationCut = "ADAMASS('B_s0')<2*GeV",
                               MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<100)")
Bs2JpsiPhi = Selection("SelBs2JpsiPhi",
                       Algorithm = _bs2JpsiPhi,
                       RequiredSelections = [Phi2KK,Jpsi2MuMu])
                       
from PhysSelPython.Wrappers import SelectionSequence
SeqBs2JpsiPhi = SelectionSequence("SeqBs2JpsiPhi",TopSelection=Bs2JpsiPhi) 

simulation = False

from DecayTreeTuple.Configuration import *
tuple = DecayTreeTuple()
tuple.Inputs = ["Phys/SelBs2JpsiPhi/Particles"]
tuple.Decay = "[B_s0 -> ^(J/psi(1S) -> ^mu+ ^mu-) ^(phi(1020) -> ^K+ ^K-)]CC"
tuple.ToolList = []
tuple.UseLabXSyntax = True
tuple.RevertToPositiveID = False

LoKi_All=tuple.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_All")
LoKi_All.Variables = {
    "lokiP" : "P",
    "lokiPT" : "PT",
}


if(simulation) : tuple.ToolList += [ "TupleToolMCTruth" ]

from Configurables import DaVinci,LHCbApp

DaVinci().DataType="2012"
DaVinci().DDDBtag="dddb-20120831"
DaVinci().CondDBtag="cond-20120831"
DaVinci().Input = ['/afs/cern.ch/work/s/shall/dsts/00020198_00012742_1.dimuon.dst']

#save an ntuple with the luminosity
DaVinci().Lumi=True
DaVinci().EvtMax = 500
DaVinci().UserAlgorithms = [ SeqBs2JpsiPhi, tuple ]
DaVinci().TupleFile = "DVNtuples.root"
DaVinci().Simulation = simulation
