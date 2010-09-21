__author__ = 'Diego Martinez Santos'
__date__ = 'June 30, 2009'

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units

#########
# J/psi #
#########

JpsiCuts = "(VFASPF(VCHI2/VDOF)<9) & (BPVVDCHI2>225)"
JpsiCuts += "& (BPVVDSIGN > 0) & (ADMASS('J/psi(1S)')<60*MeV)"

#########
# Kstar #
#########

KstCuts = "(MIPCHI2DV(PRIMARY)> 12.25) & (ADMASS('K*(892)0')<40*MeV)"
KstCuts += "& (INTREE( (ABSID=='K+') & (MIPCHI2DV(PRIMARY)  > 4.))) "
KstCuts += "& (INTREE((ABSID=='pi+') & (MIPCHI2DV(PRIMARY)  > 4.)))"

######
# Bd #
######

Bd2JpsiKstDet = CombineParticles("StripBd2JpsiKstDet")
Bd2JpsiKstDet.DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc"
Bd2JpsiKstDet.InputLocations = ["Phys/StdLooseJpsi2MuMu", "Phys/StdLooseDetachedKst2Kpi"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
Bd2JpsiKstDet.addTool( OfflineVertexFitter() )
Bd2JpsiKstDet.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bd2JpsiKstDet.OfflineVertexFitter.useResonanceVertex = False
Bd2JpsiKstDet.ReFitPVs = True
Bd2JpsiKstDet.DaughtersCuts = {"J/psi(1S)" : JpsiCuts, "K*(892)0" : KstCuts }
                               
Bd2JpsiKstDet.CombinationCut = "ADAMASS('B0') < 500.*MeV"
Bd2JpsiKstDet.MotherCut = "(MIPCHI2DV(PRIMARY)<25)"

############################################
# Create StrippingLine with this selection #
############################################

line = StrippingLine('Bd2JpsiKstDetLine'
               , prescale = 1
               , algos = [Bd2JpsiKstDet]
               )
