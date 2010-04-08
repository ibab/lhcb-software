# $Id: StrippingBs2EtacPhi.py,v 1.1 2010-04-08 14:23:11 ksendero Exp $

__author__ = ['Katarzyna Senderowska']
__date__ = '08/04/2010'
__version__ = '$Revision: 1.1 $'

'''
Bs->EtacPhi lifetime unbiased stripping selection using LoKi::Hybrid and
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
_Kaons = DataOnDemand("Kaons", Location = "Phys/StdLooseKaons")
_Pions = DataOnDemand("Pions", Location = "Phys/StdLoosePions")

### eta_c loose

###### phi for eta_c loose 
_PhiForEtacLoose = CombineParticles("PhiForEtacForBs2EtacPhiLoose")
_PhiForEtacLoose.DecayDescriptor = "phi(1020) -> K+ K-"
_PhiForEtacLoose.InputLocations = [ "StdLooseKaons" ]
_PhiForEtacLoose.DaughtersCuts = { "K+" : "(TRCHI2DOF<12) & (PIDK>-6) & ((PIDK - PIDp)>-6) & (PT>250*MeV) & (MIPCHI2DV(PRIMARY)>3)" ,
                                   "K-" : "(TRCHI2DOF<12) & (PIDK>-6) & ((PIDK - PIDp)>-6) & (PT>250*MeV) & (MIPCHI2DV(PRIMARY)>3)" }
_PhiForEtacLoose.CombinationCut = "AALL"
_PhiForEtacLoose.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
PhiForEtacLoose = Selection("PhiForEtacForBs2EtacPhiLoose", Algorithm=_PhiForEtacLoose, RequiredSelections=[_Kaons])
###### rho for eta_c loose
_RhoForEtacLoose = CombineParticles("RhoForEtacForBs2EtacPhiLoose")
_RhoForEtacLoose.DecayDescriptor = "rho(770)0 -> pi+ pi-"
_RhoForEtacLoose.InputLocations = [ "StdLoosePions" ]
_RhoForEtacLoose.DaughtersCuts = { "pi+" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-6) & (PT>250*MeV) & (MIPCHI2DV(PRIMARY)>3)" ,
                                   "pi-" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-6) & (PT>250*MeV) & (MIPCHI2DV(PRIMARY)>3)" }
_RhoForEtacLoose.CombinationCut = "AALL"
_RhoForEtacLoose.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
RhoForEtacLoose = Selection("RhoForEtacForBs2EtacPhiLoose", Algorithm=_RhoForEtacLoose, RequiredSelections=[_Pions])
###### eta_c loose
_EtacLoose = CombineParticles("EtacForBs2EtacPhiLoose")
_EtacLoose.DecayDescriptor = "eta_c(1S) -> phi(1020) rho(770)0"
_EtacLoose.InputLocations = [ "RhoForEtacForBs2EtacPhiLoose" ,
                              "PhiForEtacForBs2EtacPhiLoose" ]
_EtacLoose.DaughtersCuts = { "phi(1020)" : "ALL" ,
                             "rho(770)0" : "ALL" }
_EtacLoose.CombinationCut = "(ADAMASS('eta_c(1S)')<130.*MeV)"
_EtacLoose.MotherCut = "(PT>600*MeV) & (VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
EtacLoose = Selection("EtacForBs2EtacPhiLoose", Algorithm=_EtacLoose, RequiredSelections=[RhoForEtacLoose,PhiForEtacLoose])

### eta_c

###### phi for eta_c
_PhiForEtac = CombineParticles("PhiForEtacForBs2EtacPhi")
_PhiForEtac.DecayDescriptor = "phi(1020) -> K+ K-"
_PhiForEtac.InputLocations = [ "StdLooseKaons" ]
_PhiForEtac.DaughtersCuts = { "K+" : "(TRCHI2DOF<12) & (PIDK>-5) & ((PIDK - PIDp)>-5) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>4)",
                              "K-" : "(TRCHI2DOF<12) & (PIDK>-5) & ((PIDK - PIDp)>-5) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>4)"}
_PhiForEtac.CombinationCut = "AALL"
_PhiForEtac.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
PhiForEtac = Selection("PhiForEtacForBs2EtacPhi", Algorithm=_PhiForEtac, RequiredSelections=[_Kaons])
###### rho for eta_c
_RhoForEtac = CombineParticles("RhoForEtacForBs2EtacPhi")
_RhoForEtac.DecayDescriptor = "rho(770)0 -> pi+ pi-"
_RhoForEtac.InputLocations = [ "StdLoosePions" ]
_RhoForEtac.DaughtersCuts = { "pi+" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-5) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>4)" ,
                              "pi-" : "(TRCHI2DOF<12) & (PIDpi>-6) & ((PIDpi - PIDK)>-5) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>4)" }
_RhoForEtac.CombinationCut = "AALL"
_RhoForEtac.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0)"
RhoForEtac = Selection("RhoForEtacForBs2EtacPhi", Algorithm=_RhoForEtac, RequiredSelections=[_Pions])
###### eta_c
_Etac = CombineParticles("EtacForBs2EtacPhi")
_Etac.DecayDescriptor = "eta_c(1S) -> phi(1020) rho(770)0"
_Etac.InputLocations = [ "RhoForEtacForBs2EtacPhi" ,
                         "PhiForEtacForBs2EtacPhi" ]
_Etac.DaughtersCuts = { "phi(1020)" : "ALL" ,
                        "rho(770)0" : "ALL" }
_Etac.CombinationCut = "(ADAMASS('eta_c(1S)')<100.*MeV)"
_Etac.MotherCut = "(PT>800*MeV) & (VFASPF(VCHI2/VDOF)<25) & (BPVVDZ>0) & (MIPCHI2DV(PRIMARY)>4)"
Etac = Selection("EtacForBs2EtacPhi", Algorithm=_Etac, RequiredSelections=[RhoForEtac,PhiForEtac])


#############################################################################################
# phi selection
#
StdLoosePhi2KK = DataOnDemand("StdLoosePhi2KK", "StdLoosePhi2KK")

### phi loose
_PhiFilter = FilterDesktop("PhiFilterForBs2EtacPhiLoose")
_PhiFilter.Code = "(MAXTREE('K+'==ABSID, TRCHI2DOF)<12) & (ADMASS('phi(1020)')<35*MeV) & (PT>500*MeV) & (VFASPF(VCHI2/VDOF)<25)" 
PhiLoose = Selection("PhiFilterForBs2EtacPhiLoose", Algorithm=_PhiFilter, RequiredSelections=[StdLoosePhi2KK])

### phi
_PhiFilter = FilterDesktop("PhiFilterForBs2EtacPhi")
_PhiFilter.Code = "(MAXTREE('K+'==ABSID, TRCHI2DOF)<12) & (ADMASS('phi(1020)')<15*MeV) & (PT>700*MeV) & (VFASPF(VCHI2/VDOF)<25) & (MIPCHI2DV(PRIMARY)>4)" 
Phi = Selection("PhiFilterForBs2EtacPhi", Algorithm=_PhiFilter, RequiredSelections=[StdLoosePhi2KK])


#############################################################################################
# Bs selection
#

### Bs loose
_BsLoose = CombineParticles("Bs2EtacPhiLoose")
_BsLoose.DecayDescriptor = "B_s0 -> eta_c(1S) phi(1020)"
_BsLoose.CombinationCut = "(ADAMASS('B_s0')<300*MeV)" 
_BsLoose.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (PT>1000*MeV) & (BPVVDZ>0)" 
_BsLoose.ReFitPVs = True
#Set the OfflineVertexFitter to keep the 4 tracks and not the Etac Phi
_BsLoose.addTool( OfflineVertexFitter() )
_BsLoose.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_BsLoose.OfflineVertexFitter.useResonanceVertex = False
SelBs2EtacPhiLoose = Selection("SelBs2EtacPhiLoose", Algorithm=_BsLoose, RequiredSelections=[EtacLoose, PhiLoose])

### Bs
_Bs = CombineParticles("Bs2EtacPhi")
_Bs.DecayDescriptor = "B_s0 -> eta_c(1S) phi(1020)"
_Bs.CombinationCut = "(ADAMASS('B_s0')<300*MeV)" 
_Bs.MotherCut = "(VFASPF(VCHI2/VDOF)<16) & (PT>1200*MeV) & (BPVVDZ>0) & (BPVDIRA>0.95) "
_Bs.ReFitPVs = True
# Set the OfflineVertexFitter to keep the 4 tracks and not the Etac Phi
_Bs.addTool( OfflineVertexFitter() )
_Bs.VertexFitters.update( { "" : "OfflineVertexFitter"} )
_Bs.OfflineVertexFitter.useResonanceVertex = False
SelBs2EtacPhi = Selection("SelBs2EtacPhi", Algorithm=_Bs, RequiredSelections = [Etac, Phi])


### sequences
SeqBs2EtacPhi = SelectionSequence("SeqBs2EtacPhi", TopSelection=SelBs2EtacPhi)
SeqBs2EtacPhiLoose = SelectionSequence("SeqBs2EtacPhiLoose", TopSelection=SelBs2EtacPhiLoose)


### lines
line1 = StrippingLine('Bs2EtacPhi', prescale = 1., algos = [SeqBs2EtacPhi], postscale = 1.)
line2 = StrippingLine('Bs2EtacPhiLoose', prescale = 1., algos = [SeqBs2EtacPhiLoose], postscale = 1.)
