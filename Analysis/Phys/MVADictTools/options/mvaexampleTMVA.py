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
_phi2kk.MotherCut = "(VFASPF(VCHI2/VDOF)<100)"

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
tuple.Decay = "[B_s0 -> (^J/psi(1S) => ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-)]cc"
tuple.ToolList = []
tuple.UseLabXSyntax = True
tuple.RevertToPositiveID = False

#The traditional TupleTool
#LoKi_All=tuple.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_All")
#LoKi_All.Variables = {
#    "lokiP" : "P",
#    "lokiPT" : "PT",
#}
################################################################################
# Finished building normal selection 
################################################################################


################################################################################


################################################################################
# The NEW MVA Dictionary Tools used to implement an MVAClassifier
################################################################################
# Imports
from Configurables import LoKi__Hybrid__MultiToolDictTool
from Configurables import LoKi__Hybrid__Dict2TupleTool
from Configurables import LoKi__Hybrid__DictTransformTool_TMVATransform_ as TMVAClassifier
#We are going to add the MVA tools to the phi
tuple.addBranches({
    "Phi" : "B_s0 -> (^phi(1020) -> K+ K-) ? ",
    })

# we are adding the Dict2TupleTool to the Phi branch. 
# this will write the MVA classifier response dictionary into the ntuple
# All variables in the dict will be prefixed with "Phi_" as they are added to the ntuple
Phi_tmva=tuple.Phi.addTupleTool(LoKi__Hybrid__Dict2TupleTool, "TMVA2Tuple")

# Add the TMVAClassifier to the Dict2TupleTool
Phi_tmva.addTool(TMVAClassifier,"TMVA")
Phi_tmva.Source = "LoKi__Hybrid__DictTransformTool_TMVATransform_/TMVA"
# Configure the classifier (options depend on which classifier is used)
Phi_tmva.TMVA.Options = {
    "Name"       : "MyBDT",           # Name for the MVA response variable
    "XMLFile"    : "TestPhi2KK.xml",  # TMVA uses an xml file to load the classifier
    "KeepVars"   : "1",
}
# Note that other dictionary transformations can easily be added by
# implementing a new DictTransformationTool using the DictTransformation policies
# see Phys/LoKiArrayFunctors/src/Components/DummyTransform.cpp for a prototype


# Add a MultiToolDictTool as the source of the classifier
# the MultiTool will use LoKiFunctors to query the variables needed  
Phi_tmva.TMVA.addTool(LoKi__Hybrid__MultiToolDictTool,"MVAdict3")
Phi_tmva.TMVA.Source = "LoKi__Hybrid__MultiToolDictTool/MVAdict3"
# the variable names have to correspond exactly to what is needed by the classifier 
# the prefixing with the node names has to be done manually here!
Phi_tmva.TMVA.MVAdict3.Variables = {
    "lab1_PT"               : "PT",
    "lab1_IPCHI2_OWNPV"     : "MIPCHI2DV(PRIMARY)",
    "lab2_PT"               : "CHILD(PT,1)",
    "lab3_PT"               : "CHILD(PT,2)",
    "lab2_IPCHI2_OWNPV"     : "CHILD(MIPCHI2DV(PRIMARY),1)",
    "lab3_IPCHI2_OWNPV"     : "CHILD(MIPCHI2DV(PRIMARY),2)",
    }
# CombineParticles.MotherCut = "BPVIPCHI2()<25"


# How to write a dictionary directly into the ntuple:
#tuple.addTupleTool(LoKi__Hybrid__Dict2TupleTool, "MVATuple")
#tuple.MVATuple.addTool(LoKi__Hybrid__MultiToolDictTool, "MVADict2Tuple")
#tuple.MVATuple.Source = "MVADict2Tuple"
#tuple.MVATuple.MVADict2Tuple.Variables = variables




################################################################################
# End of MVAClassifier TupleTool
################################################################################

    

if(simulation) : tuple.ToolList += [ "TupleToolMCTruth" ]

from Configurables import DaVinci,LHCbApp

DaVinci().DataType="2012"
DaVinci().DDDBtag="dddb-20120831"
DaVinci().CondDBtag="cond-20120831"
DaVinci().Input = ['/afs/cern.ch/work/s/shall/dsts/00020198_00012742_1.dimuon.dst']

#save an ntuple with the luminosity
DaVinci().Lumi=True
DaVinci().EvtMax = 500
DaVinci().UserAlgorithms = [SeqBs2JpsiPhi, tuple ]
DaVinci().TupleFile = "DVNtuples.root"
DaVinci().Simulation = simulation
