# $Id: StrippingBd2JpsiKS.py,v 1.4 2009-09-04 16:02:55 poluekt Exp $

__author__ = 'Greig Cowan'
__date__ = '20/05/2009'
__version__ = '$Revision: 1.4 $'

'''
Bd->JpsiKS stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units

################
# KS long long #
################
KSLLForBd2Jpsi2MuMuKS = FilterDesktop("StripKSLLForBd2Jpsi2MuMuKS")
KSLLForBd2Jpsi2MuMuKS.InputLocations = ["StdLooseKsLL"]
KSLLForBd2Jpsi2MuMuKS.Code = "  (MINTREE( ('pi+'==ABSID), P) >2.*GeV)"\
                             "& (MINTREE( ('pi+'==ABSID), MIPCHI2DV(PRIMARY)) >9.)"\
                             "& (MAXTREE( ('pi+'==ABSID), TRCHI2DOF) <10.)"\
                             "& (VFASPF(VCHI2/VDOF)<10.)"\
                             "& (ADMASS('KS0') < 80.0*MeV)"

############################
# KS downstream downstream #
############################
KSDDForBd2Jpsi2MuMuKS = FilterDesktop("StripKSDDForBd2Jpsi2MuMuKS")
KSDDForBd2Jpsi2MuMuKS.InputLocations = ["StdLooseKsDD"]
KSDDForBd2Jpsi2MuMuKS.Code = "  (MINTREE( ('pi+'==ABSID), P) >2.*GeV)"\
                             "& (MINTREE( ('pi+'==ABSID), MIPCHI2DV(PRIMARY)) >4.)"\
                             "& (MAXTREE( ('pi+'==ABSID), TRCHI2DOF) <20.)"\
                             "& (VFASPF(VCHI2/VDOF)<20.)"\
                             "& (ADMASS('KS0') < 100.0*MeV)"

################
# Bd -> JpsiKS #
################
Bd2JpsiKS = CombineParticles("StripBd2JpsiKS")
Bd2JpsiKS.DecayDescriptor = "[B0 -> J/psi(1S) KS0]cc"
Bd2JpsiKS.InputLocations = ["StripKSLLForBd2Jpsi2MuMuKS",
                            "StripKSDDForBd2Jpsi2MuMuKS",
                            "StdLTUnbiasedJpsi2MuMu"]

Bd2JpsiKS.addTool( OfflineVertexFitter() )
Bd2JpsiKS.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bd2JpsiKS.OfflineVertexFitter.useResonanceVertex = False
Bd2JpsiKS.ReFitPVs = True

Bd2JpsiKS.DaughtersCuts = {'KS0' : 'PT>1.*GeV',
                           'J/psi(1S)' : 'PT>1.*GeV'}
Bd2JpsiKS.CombinationCut = "ADAMASS('B0') < 300.*MeV"
Bd2JpsiKS.MotherCut = "(VFASPF(VCHI2/VDOF) < 15.) & (BPVIPCHI2() < 25.)"

############################################
# Create StrippingLine with this selection #
############################################
Bd2JpsiKSLine = StrippingLine('Bd2JpsiKSLine'
               , prescale = 1
               , algos = [KSLLForBd2Jpsi2MuMuKS, KSDDForBd2Jpsi2MuMuKS, Bd2JpsiKS]
               , stream = 'Bmuon'
               )
