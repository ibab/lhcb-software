# $Id: StrippingBs2JpsiPhi.py,v 1.5 2009-09-04 16:02:55 poluekt Exp $

__author__ = 'Greig Cowan'
__date__ = '20/05/2009'
__version__ = '$Revision: 1.5 $'

'''
Bs->JpsiPhi stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units

#############################
# Standard Unbiased Phi->KK #
#############################
StdUnbiasedPhi2KK = FilterDesktop("StripStdUnbiasedPhi2KK")
StdUnbiasedPhi2KK.InputLocations = ["StdLoosePhi2KK"]
StdUnbiasedPhi2KK.Code = "  (MINTREE('K+'==ABSID, PIDK) > -5.0)" \
			 "& (MINTREE('K+'==ABSID, PT) > 500.0)"\
                         "& (ADMASS('phi(1020)') < 15.*MeV)" \
                         "& (PT > 1000.*MeV)" \
                         "& (VFASPF(VCHI2/VDOF) < 20.0)"
######
# Bs #
######
Bs2JpsiPhi = CombineParticles("StripBs2JpsiPhi")
Bs2JpsiPhi.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
Bs2JpsiPhi.InputLocations = ["StdLTUnbiasedJpsi2MuMu",
                             "StripStdUnbiasedPhi2KK"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Phi
Bs2JpsiPhi.addTool( OfflineVertexFitter() )
Bs2JpsiPhi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bs2JpsiPhi.OfflineVertexFitter.useResonanceVertex = False
Bs2JpsiPhi.ReFitPVs = True

# Basic + loose offline PID
Bs2JpsiPhi.CombinationCut = "ADAMASS('B_s0') < 300.*MeV"
Bs2JpsiPhi.MotherCut = "(VFASPF(VCHI2/VDOF) < 5.) & (BPVIPCHI2() < 25.)"

############################################
# Create StrippingLine with this selection #
############################################
Bs2JpsiPhiLine = StrippingLine('Bs2JpsiPhiLine'
               , prescale = 1
               , algos = [StdUnbiasedPhi2KK, Bs2JpsiPhi]
               , stream = 'Bmuon'
               )
