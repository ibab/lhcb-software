# $Id: StrippingBd2JpsiKSBiased.py,v 1.2 2009-09-04 16:02:55 poluekt Exp $

__author__ = 'Fernando Rodrigues'
__date__ = '28/08/2009'
__version__ = '$Revision: 1.2 $'

'''
Bd->JpsiKS biased stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units

##################
# J/Psi -> mu mu #
##################
JpsiForBd2Jpsi2MuMuKS = FilterDesktop("JpsiForBd2Jpsi2MuMuKS")
JpsiForBd2Jpsi2MuMuKS.InputLocations = ["StdLooseJpsi2MuMu"]
JpsiForBd2Jpsi2MuMuKS.Code = "  (MINTREE( ('mu+'==ABSID), PT) > 0.3*GeV)"\
                             "& (MAXTREE( ('mu+'==ABSID), TRCHI2DOF) < 6.)"\
                             "& (VFASPF(VCHI2/VDOF)<25.)"\
                             "& (ADMASS('J/psi(1S)') < 100.0*MeV)"

################
# KS long long #
################
KSLLForBd2Jpsi2MuMuKS = FilterDesktop("KSLLForBd2Jpsi2MuMuKS")
KSLLForBd2Jpsi2MuMuKS.InputLocations = ["StdLooseKsLL"]
KSLLForBd2Jpsi2MuMuKS.Code = "  (MINTREE( ('pi+'==ABSID), P) >2.*GeV)"\
                             "& (MAXTREE( ('pi+'==ABSID), TRCHI2DOF) <20.)"\
                             "& (VFASPF(VCHI2/VDOF)<30.)"\
                             "& (ADMASS('KS0') < 35.0*MeV)"

############################
# KS downstream downstream #
############################
KSDDForBd2Jpsi2MuMuKS = FilterDesktop("KSDDForBd2Jpsi2MuMuKS")
KSDDForBd2Jpsi2MuMuKS.InputLocations = ["StdLooseKsDD"]
KSDDForBd2Jpsi2MuMuKS.Code = "  (MINTREE( ('pi+'==ABSID), P) >2.*GeV)"\
                             "& (MAXTREE( ('pi+'==ABSID), TRCHI2DOF) <20.)"\
                             "& (VFASPF(VCHI2/VDOF)<30.)"\
                             "& (ADMASS('KS0') < 64.0*MeV)"

#########################
# Bd -> JpsiKS (Biased) #
#########################
Bd2JpsiKSBiased = CombineParticles("Bd2JpsiKSBiased")
Bd2JpsiKSBiased.DecayDescriptor = "[B0 -> J/psi(1S) KS0]cc"
Bd2JpsiKSBiased.InputLocations = ["JpsiForBd2Jpsi2MuMuKS",
                                  "KSLLForBd2Jpsi2MuMuKS",
                                  "KSDDForBd2Jpsi2MuMuKS"]
Bd2JpsiKSBiased.addTool( OfflineVertexFitter() )
Bd2JpsiKSBiased.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bd2JpsiKSBiased.OfflineVertexFitter.useResonanceVertex = False
Bd2JpsiKSBiased.ReFitPVs = True

Bd2JpsiKSBiased.CombinationCut = "ADAMASS('B0') < 300.*MeV"
Bd2JpsiKSBiased.MotherCut = "(BPVDIRA>0.999)"

############################################
# Create StrippingLine with this selection #
############################################
Bd2JpsiKSBiasedLine = StrippingLine('Bd2JpsiKSBiasedLine'
               , prescale = 1
               , algos = [JpsiForBd2Jpsi2MuMuKS, KSLLForBd2Jpsi2MuMuKS, KSDDForBd2Jpsi2MuMuKS, Bd2JpsiKSBiased]
               , stream = 'Bmuon'
               )
