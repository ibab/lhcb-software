# $Id: StrippingBd2JpsiKstar.py,v 1.1 2009-05-20 15:41:24 gcowan Exp $

__author__ = 'Greig Cowan'
__date__ = '20/05/2009'
__version__ = '$Revision: 1.1 $'

'''
Bd->JpsiK* stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units

#######
# Kst #
#######
StUnbiasedKst2Kpi = CombineParticles("StripStdUnbiasedKst2Kpi")
StUnbiasedKst2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
StUnbiasedKst2Kpi.InputLocations = ["StdLooseKaons",
                                    "StdLoosePions"]
StUnbiasedKst2Kpi.DaughtersCuts = {"K+" :
                                   "  (MINTREE('K+'==ABSID, (PIDK - PIDp)) >-6.)"\
                                   "& (MINTREE('K+'==ABSID, PIDK) > -5.)"
                                   }
StUnbiasedKst2Kpi.CombinationCut = "(ADAMASS('K*(892)0') < 90.*MeV)"
StUnbiasedKst2Kpi.MotherCut = "(VFASPF(VCHI2/VDOF)<20) & (PT > 1000.*MeV)"

######
# Bd #
######
Bd2JpsiKst = CombineParticles("StripBd2JpsiKst")
Bd2JpsiKst.DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc"
Bd2JpsiKst.InputLocations = ["StdUnbiasedJpsi2MuMu",
                             "StripStdUnbiasedKst2Kpi"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
Bd2JpsiKst.addTool( OfflineVertexFitter() )
Bd2JpsiKst.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bd2JpsiKst.OfflineVertexFitter.useResonanceVertex = False
Bd2JpsiKst.ReFitPVs = True

Bd2JpsiKst.CombinationCut = "ADAMASS('B0') < 300.*MeV"
Bd2JpsiKst.MotherCut = "(VFASPF(VCHI2/VDOF)<5) & (PT > 2000.*MeV)"

############################################
# Create StrippingLine with this selection #
############################################
Bd2JpsiKstLine = StrippingLine('Bd2JpsiKstLine'
               , prescale = 1
               , algos = [StdUnbiasedKst2Kpi, Bd2JpsiKst]
               , stream = 'BExclusive'
               )
