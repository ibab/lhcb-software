# $Id: StrippingBs2ChicPhi.py,v 1.2 2010-05-10 12:12:05 jpalac Exp $

__author__ = ['Katarzyna Senderowska']
__date__ = '08/04/2010'
__version__ = '$Revision: 1.2 $'

'''
Bs->ChicPhi lifetime unbiased stripping selection using LoKi::Hybrid and
python configurables. PV refitting is done. Implements the nominal and 
loose lifetime unbiased stripping selections.
'''

from os import environ
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter	
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine, StrippingMember


#############################################################################################
# eta_c selection
#
_Kaons = DataOnDemand(Location = "Phys/StdLooseKaons")
_Pions = DataOnDemand(Location = "Phys/StdLoosePions")

### chi_c0 loose 

###### rho for chi_c0 loose
_RhoForChicLoose = CombineParticles("RhoForChicForBs2ChicPhiLoose")
_RhoForChicLoose.DecayDescriptor = "rho(770)0 -> pi+ pi-"
_RhoForChicLoose.InputLocations = [ "StdLoosePions" ]
_RhoForChicLoose.DaughtersCuts = { "pi+" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-6) & (PT>250*MeV) & (MIPCHI2DV(PRIMARY)>3)" ,
                                   "pi-" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-6) & (PT>250*MeV) & (MIPCHI2DV(PRIMARY)>3)" }
_RhoForChicLoose.CombinationCut = "AALL"
_RhoForChicLoose.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
RhoForChicLoose = Selection("RhoForChicForBs2ChicPhiLoose", Algorithm=_RhoForChicLoose, RequiredSelections=[_Pions])
###### chi_c0 loose
_ChicLoose = CombineParticles("ChicForBs2ChicPhiLoose")
_ChicLoose.DecayDescriptor = "chi_c0(1P) -> rho(770)0 rho(770)0"
_ChicLoose.InputLocations = [ "RhoForChicForBs2ChicPhiLoose" ]
_ChicLoose.DaughtersCuts = { "rho(770)0" : "ALL" }
_ChicLoose.CombinationCut = "(ADAMASS('chi_c0(1P)')<130.*MeV)"
_ChicLoose.MotherCut = "(PT>700*MeV) & (VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
ChicLoose = Selection("ChicForBs2ChicPhiLoose", Algorithm=_ChicLoose, RequiredSelections=[RhoForChicLoose])

### chi_c0 

###### rho for chi_c0
_RhoForChic = CombineParticles("RhoForChicForBs2ChicPhi")
_RhoForChic.DecayDescriptor = "rho(770)0 -> pi+ pi-"
_RhoForChic.InputLocations = [ "StdLoosePions" ]
_RhoForChic.DaughtersCuts = { "pi+" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-5) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>4)" ,
                              "pi-" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-5) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>4)" }
_RhoForChic.CombinationCut = "AALL"
_RhoForChic.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
RhoForChic = Selection("RhoForChicForBs2ChicPhi", Algorithm=_RhoForChic, RequiredSelections=[_Pions])
###### chi_c0
_Chic = CombineParticles("ChicForBs2ChicPhi")
_Chic.DecayDescriptor = "chi_c0(1P) -> rho(770)0 rho(770)0"
_Chic.InputLocations = [ "RhoForChicForBs2ChicPhi" ]
_Chic.DaughtersCuts = { "rho(770)0" : "ALL" }
_Chic.CombinationCut = "(ADAMASS('chi_c0(1P)')<100.*MeV)"
_Chic.MotherCut = "(PT>900*MeV) & (VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0) & (MIPCHI2DV(PRIMARY)>4)"
Chic = Selection("ChicForBs2ChicPhi", Algorithm=_Chic, RequiredSelections=[RhoForChic])


#############################################################################################
# phi selection
#
StdLoosePhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")

### phi loose
_PhiFilter = FilterDesktop("PhiFilterForBs2ChicPhiLoose")
_PhiFilter.Code = "(MAXTREE('K+'==ABSID, TRCHI2DOF)<12) & (ADMASS('phi(1020)')<35*MeV) & (PT>500*MeV) & (VFASPF(VCHI2/VDOF)<25)" 
PhiLoose = Selection("PhiFilterForBs2ChicPhiLoose", Algorithm=_PhiFilter, RequiredSelections=[StdLoosePhi2KK])

### phi
_PhiFilter = FilterDesktop("PhiFilterForBs2ChicPhi")
_PhiFilter.Code = "(MAXTREE('K+'==ABSID, TRCHI2DOF)<12) & (ADMASS('phi(1020)')<15*MeV) & (PT>800*MeV) & (VFASPF(VCHI2/VDOF)<25) & (MIPCHI2DV(PRIMARY)>4)" 
Phi = Selection("PhiFilterForBs2ChicPhi", Algorithm=_PhiFilter, RequiredSelections=[StdLoosePhi2KK])


#############################################################################################
# Bs selection
#

### Bs loose
_BsLoose = CombineParticles("Bs2ChicPhiLoose")
_BsLoose.DecayDescriptor = "B_s0 -> chi_c0(1P) phi(1020)"
_BsLoose.CombinationCut = "(ADAMASS('B_s0')<300*MeV)" 
_BsLoose.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (PT>1000*MeV) & (BPVVDZ>0)" 
_BsLoose.ReFitPVs = True
#Set the OfflineVertexFitter to keep the 6 tracks and not the Chic Phi
_BsLoose.addTool( OfflineVertexFitter() )
_BsLoose.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_BsLoose.OfflineVertexFitter.useResonanceVertex = False
SelBs2ChicPhiLoose = Selection("SelBs2ChicPhiLoose", Algorithm=_BsLoose, RequiredSelections=[ChicLoose, PhiLoose])

### Bs
_Bs = CombineParticles("Bs2ChicPhi")
_Bs.DecayDescriptor = "B_s0 -> chi_c0(1P) phi(1020)"
_Bs.CombinationCut = "(ADAMASS('B_s0')<300*MeV)" 
_Bs.MotherCut = "(VFASPF(VCHI2/VDOF)<16) & (PT>1500*MeV) & (BPVVDZ>0) & (BPVDIRA>0.995)"
_Bs.ReFitPVs = True
# Set the OfflineVertexFitter to keep the 6 tracks and not the Chic Phi
_Bs.addTool( OfflineVertexFitter() )
_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_Bs.OfflineVertexFitter.useResonanceVertex = False
SelBs2ChicPhi = Selection("SelBs2ChicPhi", Algorithm=_Bs, RequiredSelections = [Chic, Phi])


### sequences
SeqBs2ChicPhi = SelectionSequence("SeqBs2ChicPhi", TopSelection=SelBs2ChicPhi)
SeqBs2ChicPhiLoose = SelectionSequence("SeqBs2ChicPhiLoose", TopSelection=SelBs2ChicPhiLoose)


### lines
line1 = StrippingLine('Bs2ChicPhi', prescale = 1., algos = [SeqBs2ChicPhi], postscale = 1.)
line2 = StrippingLine('Bs2ChicPhiLoose', prescale = 1., algos = [SeqBs2ChicPhiLoose], postscale = 1.)
