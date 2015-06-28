#!/usr/bin/env python
# =============================================================================
# $Id: CharmTaggerLists.py
# =============================================================================
## @file  FlavourTagging/CharmTaggerLists.py
# input lines for OS charm tagger
# =============================================================================
"""
configuration file
"""
__author__  = "Riccardo Cenci, Jack Wimberley"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.0 $"
# =============================================================================

# =============================================================================
__all__ = (
    'CharmPreambulo',
    #'Tag_CharmRecSeq'
    #'Tag_PVFilter',
    'Tag_CharmElecList',
    'Tag_CharmMuonList',
    'Tag_CharmPionList',
    'Tag_CharmPi0List',
    'Tag_CharmKaonList',
    'Tag_CharmTightKaonList',
    'Tag_CharmProtonList',
    'Tag_StdD02KPi',
    'Tag_StdD02KPiPiPi',
    'Tag_StdDp2KPiPi',
    'Tag_StdD02KPiPi0',
    'Tag_StdD2KPipart',
    'Tag_StdD2Kepart',
    'Tag_StdD2Kmupart',
    'Tag_StdLambdaC2PKPi',
    'locations'
    )

# =============================================================================

from Gaudi.Configuration import *
#from Configurables import GaudiSequencer
from Configurables import CombineParticles
from Configurables import FilterDesktop
#from Configurables import LoKi__VoidFilter
from CommonParticles.Utils import *

#Tag_CharmRecSeq = GaudiSequencer("Tag_CharmRecSeq")

#PVList = "Rec/Vertex/Primary"
#Tag_PVFilter = LoKi__VoidFilter("Tag_PVFilter")
#Tag_PVFilter.Code = "CONTAINS('Rec/Vertex/Primary')>0"
#locations = updateDoD(Tag_PVFilter)
#Tag_CharmRecSeq.Members += [Tag_PVFilter]

InputElecList = "Phys/StdAllNoPIDsElectrons/Particles"
InputMuonList = "Phys/StdAllNoPIDsMuons/Particles"
InputPionList = "Phys/StdAllNoPIDsPions/Particles"
InputMergedPi0List = "Phys/StdLooseMergedPi0/Particles"
InputResolvedPi0List = "Phys/StdLooseResolvedPi0/Particles"
InputKaonList = "Phys/StdAllNoPIDsKaons/Particles"
InputProtonList = "Phys/StdAllNoPIDsProtons/Particles"

OutputElecList = "Phys/Tag_CharmElecList/Particles"
OutputMuonList = "Phys/Tag_CharmMuonList/Particles"
OutputPionList = "Phys/Tag_CharmPionList/Particles"
OutputPi0List = "Phys/Tag_CharmPi0List/Particles"
OutputKaonList = "Phys/Tag_CharmKaonList/Particles"
OutputTightKaonList = "Phys/Tag_CharmTightKaonList/Particles"
OutputKSList = "Phys/Tag_CharmKSList/Particles"
OutputProtonList = "Phys/Tag_CharmProtonList/Particles"

CharmPreambulo = [
    "from LoKiPhysMC.decorators import *" ,
    "from LoKiPhysMC.functions import mcMatch",
    "from GaudiKernel.SystemOfUnits import *"
    ]

# =============================================================================

# PARTICLE LISTS #

Tag_CharmElecList = FilterDesktop("Tag_CharmElecList")
Tag_CharmElecList.Code = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2() > 4.0) & (PROBNNe > 0.05) & (PROBNNmu < 0.35) & (PROBNNghost < 0.35)" 
Tag_CharmElecList.Inputs = [InputElecList]
Tag_CharmElecList.Output = OutputElecList
locations = updateDoD(Tag_CharmElecList)
#Tag_CharmRecSeq.Members += [Tag_CharmElecList]

Tag_CharmMuonList = FilterDesktop("Tag_CharmMuonList")
Tag_CharmMuonList.Code = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2() > 4.0) & (PROBNNmu > 0.05) & (PROBNNe < 0.35) & (PROBNNghost < 0.35)" 
Tag_CharmMuonList.Inputs = [InputMuonList]
Tag_CharmMuonList.Output = OutputMuonList
locations.update(updateDoD(Tag_CharmMuonList))
#Tag_CharmRecSeq.Members += [Tag_CharmMuonList]

Tag_CharmPionList = FilterDesktop("Tag_CharmPionList")
Tag_CharmPionList.Code = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2() > 4.0) & (PROBNNK < 0.35) & (PROBNNghost < 0.35)" 
Tag_CharmPionList.Inputs = [InputPionList]
Tag_CharmPionList.Output = OutputPionList
locations.update(updateDoD(Tag_CharmPionList))
#Tag_CharmRecSeq.Members += [Tag_CharmPionList]

Tag_CharmPi0List = FilterDesktop("Tag_CharmPi0List")
Tag_CharmPi0List.Code = "(ABSID==111) & (PT>500*MeV) & (P>1000*MeV)"
Tag_CharmPi0List.Inputs = [InputMergedPi0List,InputResolvedPi0List]
Tag_CharmPi0List.Output = OutputPi0List
locations.update(updateDoD(Tag_CharmPi0List))
#Tag_CharmRecSeq.Members += [Tag_CharmPi0List]

Tag_CharmKaonList = FilterDesktop("Tag_CharmKaonList")
Tag_CharmKaonList.Code = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2() > 4.0) & (PROBNNK > 0.05) & (PROBNNghost < 0.35)" 
Tag_CharmKaonList.Inputs = [InputKaonList]
Tag_CharmKaonList.Output = OutputKaonList
locations.update(updateDoD(Tag_CharmKaonList))
#Tag_CharmRecSeq.Members += [Tag_CharmKaonList]

Tag_CharmTightKaonList = FilterDesktop("Tag_CharmTightKaonList")
Tag_CharmTightKaonList.Code = "(MIPCHI2DV(PRIMARY)>10.0)"
Tag_CharmTightKaonList.Inputs = [OutputKaonList]
Tag_CharmTightKaonList.Output = OutputTightKaonList
locations.update(updateDoD(Tag_CharmTightKaonList))
#Tag_CharmRecSeq.Members += [Tag_CharmTightKaonList]

Tag_CharmProtonList = FilterDesktop("Tag_CharmProtonList")
Tag_CharmProtonList.Code = "(PT>100*MeV) & (P>1000*MeV) & (BPVIPCHI2() > 4.0) & (PROBNNp > 0.05) &(PROBNNK < 0.35) & (PROBNNghost < 0.35)" 
Tag_CharmProtonList.Inputs = [InputProtonList]
Tag_CharmProtonList.Output = OutputProtonList
locations.update(updateDoD(Tag_CharmProtonList))
#Tag_CharmRecSeq.Members += [Tag_CharmProtonList]

D0CombCut = "(ADAMASS('D0')<100*MeV) & (ADOCAMAX('')<0.5*mm)"
DpCombCut = "(ADAMASS('D+')<100*MeV) & (ADOCAMAX('')<0.5*mm)"
DstCombCut = "(ADAMASS('D*(2010)+')<50*MeV) & (ADOCAMAX('')<0.5*mm)"
LCCombCut = "(ADAMASS('Lambda_c+')<110*MeV) & (ADOCAMAX('')<0.5*mm)"

DMotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2>25) & (BPVDIRA>0.998)"
D0MotherCut = "(ADMASS('D0')<50*MeV) & " + DMotherCut
DpMotherCut = "(ADMASS('D+')<50*MeV) & " + DMotherCut
DstMotherCut = "(M-MAXTREE('D0'==ABSID,M)<165.5) & (VFASPF(VCHI2/VDOF)<25) & (BPVVDCHI2>25) & (BPVDIRA>0.998)"
LCMotherCut = "(ADMASS('Lambda_c+')<100*MeV) & (VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2>25) & (BPVDIRA>0.998)"

# =============================================================================
# D0 -> K Pi #

Tag_StdD02KPi = CombineParticles("Tag_StdD02KPi")
Tag_StdD02KPi.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdD02KPi.Preambulo = CharmPreambulo
Tag_StdD02KPi.DecayDescriptors = ["[D0 -> K- pi+]cc"]
Tag_StdD02KPi.CombinationCut = D0CombCut
Tag_StdD02KPi.MotherCut = D0MotherCut
Tag_StdD02KPi.Inputs = [OutputKaonList, OutputPionList]
Tag_StdD02KPi.Output = "Phys/" + Tag_StdD02KPi.name() + "/Particles"
locations.update(updateDoD(Tag_StdD02KPi))
#Tag_CharmRecSeq.Members += [Tag_StdD02KPi]

# =============================================================================
# D0 -> K Pi Pi Pi #

Tag_StdD02KPiPiPi = CombineParticles ("Tag_StdD02KPiPiPi")
Tag_StdD02KPiPiPi.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdD02KPiPiPi.Preambulo = CharmPreambulo
Tag_StdD02KPiPiPi.DecayDescriptors = ["[D0 -> K- pi+ pi+ pi-]cc"]
Tag_StdD02KPiPiPi.DaughtersCuts = {
    "K+" : "(PT>150*MeV) & (PROBNNK > 0.1) & (BPVIPCHI2()>6.0)"
    , "pi+" : "(PT>150*MeV) & (BPVIPCHI2()>6.0)"
    , "pi-" : "(PT>150*MeV) & (BPVIPCHI2()>6.0)"
    }
Tag_StdD02KPiPiPi.CombinationCut = D0CombCut
Tag_StdD02KPiPiPi.MotherCut = D0MotherCut
Tag_StdD02KPiPiPi.Inputs = [OutputKaonList, OutputPionList]
Tag_StdD02KPiPiPi.Output = "Phys/" + Tag_StdD02KPiPiPi.name() + "/Particles"
locations.update(updateDoD(Tag_StdD02KPiPiPi))
#Tag_CharmRecSeq.Members += [Tag_StdD02KPiPiPi]

# =============================================================================
# D+ -> K Pi Pi #

Tag_StdDp2KPiPi = CombineParticles("Tag_StdDp2KPiPi")
Tag_StdDp2KPiPi.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdDp2KPiPi.Preambulo = CharmPreambulo
Tag_StdDp2KPiPi.DecayDescriptors = ["[D+ -> K- pi+ pi+]cc"]
Tag_StdDp2KPiPi.CombinationCut = DpCombCut
Tag_StdDp2KPiPi.MotherCut = DpMotherCut
Tag_StdDp2KPiPi.Inputs = [OutputKaonList, OutputPionList]
Tag_StdDp2KPiPi.Output = "Phys/" + Tag_StdDp2KPiPi.name() + "/Particles"
locations.update(updateDoD(Tag_StdDp2KPiPi))
#Tag_CharmRecSeq.Members += [Tag_StdDp2KPiPi]

# =============================================================================
# D0 -> K Pi Pi0 #

Tag_StdD02KPiPi0 = CombineParticles("Tag_StdD02KPiPi0")
Tag_StdD02KPiPi0.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdD02KPiPi0.Preambulo = CharmPreambulo
Tag_StdD02KPiPi0.DecayDescriptors = ["[D0 -> K- pi+ pi0]cc"]
Tag_StdD02KPiPi0.CombinationCut = D0CombCut
Tag_StdD02KPiPi0.MotherCut = D0MotherCut
Tag_StdD02KPiPi0.Inputs = [OutputKaonList, OutputPionList, OutputPi0List]
Tag_StdD02KPiPi0.Output = "Phys/" + Tag_StdD02KPiPi0.name() + "/Particles"
locations.update(updateDoD(Tag_StdD02KPiPi0))
#Tag_CharmRecSeq.Members += [Tag_StdD02KPiPi0]

# =============================================================================
# D0 / D+ -> K Pi partial #

Tag_StdD2KPipart = CombineParticles("Tag_StdD2KPipart")
Tag_StdD2KPipart.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdD2KPipart.Preambulo = CharmPreambulo
Tag_StdD2KPipart.DecayDescriptors = ["[D0 -> K- pi+]cc"]
Tag_StdD2KPipart.DaughtersCuts = { "pi+" : "(MIPCHI2DV(PRIMARY)>10.0)" }
Tag_StdD2KPipart.CombinationCut = "( in_range(1450*MeV,AWM('K-','pi+'),1850*MeV) | in_range(842*MeV,AWM('K-','pi+'),955*MeV))"
Tag_StdD2KPipart.MotherCut = "( (ADMASS('K*(892)0')<50*MeV) | ( (DMASS('D0')>-400*MeV) & (DMASS('D0')<0*MeV)))" + " & " + DMotherCut
Tag_StdD2KPipart.Inputs = [OutputTightKaonList, OutputPionList]
Tag_StdD2KPipart.Output = "Phys/" + Tag_StdD2KPipart.name() + "/Particles"
locations.update(updateDoD(Tag_StdD2KPipart))
#Tag_CharmRecSeq.Members += [Tag_StdD2KPipart]

# =============================================================================
# D0 / D+ -> K e X #

Tag_StdD2Kepart = CombineParticles("Tag_StdD2Kepart")
Tag_StdD2Kepart.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdD2Kepart.Preambulo = CharmPreambulo
Tag_StdD2Kepart.DecayDescriptors = ["[D0 -> K- e+]cc"]
Tag_StdD2Kepart.DaughtersCuts = { "e+" : "(MIPCHI2DV(PRIMARY)>10.0)" }
Tag_StdD2Kepart.CombinationCut = "in_range(1450*MeV,AWM('K-','e+'),1850*MeV)"
Tag_StdD2Kepart.MotherCut = "(DMASS('D0')>-400*MeV) & (DMASS('D0')<0*MeV) & " + DMotherCut
Tag_StdD2Kepart.Inputs = [OutputTightKaonList, OutputElecList]
Tag_StdD2Kepart.Output = "Phys/" + Tag_StdD2Kepart.name() + "/Particles"
locations.update(updateDoD(Tag_StdD2Kepart))
#Tag_CharmRecSeq.Members += [Tag_StdD2Kepart]

# =============================================================================
# D0 / D+ -> K mu X #

Tag_StdD2Kmupart = CombineParticles("Tag_StdD2Kmupart")
Tag_StdD2Kmupart.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdD2Kmupart.Preambulo = CharmPreambulo
Tag_StdD2Kmupart.DecayDescriptors = ["[D0 -> K- mu+]cc"]
Tag_StdD2Kmupart.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)>10.0)" }
Tag_StdD2Kmupart.CombinationCut = "in_range(1450*MeV,AWM('K-','mu+'),1850*MeV)"
Tag_StdD2Kmupart.MotherCut = "(DMASS('D0')>-400*MeV) & (DMASS('D0')<0*MeV) & " + DMotherCut
Tag_StdD2Kmupart.Output = "Phys/" + Tag_StdD2Kmupart.name() + "/Particles"
Tag_StdD2Kmupart.Inputs = [OutputTightKaonList, OutputMuonList]
locations.update(updateDoD(Tag_StdD2Kmupart))
#Tag_CharmRecSeq.Members += [Tag_StdD2Kmupart]

# =============================================================================
# Lambda_c -> P K Pi #

Tag_StdLambdaC2PKPi = CombineParticles("Tag_StdLambdaC2PKPi")
Tag_StdLambdaC2PKPi.ParticleCombiners = {'' : 'LoKi::VertexFitter' }
Tag_StdLambdaC2PKPi.Preambulo = CharmPreambulo
Tag_StdLambdaC2PKPi.DecayDescriptors = ["[Lambda_c+ -> p+ K- pi+]cc"]
Tag_StdLambdaC2PKPi.CombinationCut = LCCombCut
Tag_StdLambdaC2PKPi.MotherCut = LCMotherCut
Tag_StdLambdaC2PKPi.Inputs = [OutputKaonList, OutputProtonList, OutputPionList]
Tag_StdLambdaC2PKPi.Output = "Phys/" + Tag_StdLambdaC2PKPi.name() + "/Particles"
locations.update(updateDoD(Tag_StdLambdaC2PKPi))
#Tag_CharmRecSeq.Members += [Tag_StdLambdaC2PKPi]

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD(locations)

# =============================================================================
# The END
# =============================================================================
  
