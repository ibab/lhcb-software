__author__ = 'Celestino Rodriguez Cobo'
__date__ = 'October 6, 2009'
__version__='1.0'

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units



#########
# Kstar #
#########

KstCuts ="(VFASPF(VCHI2/VDOF)<25) & (ADMASS('K*(892)0')<150*MeV)"
KstCuts+= "&(PT> 750*MeV) &(MIPCHI2DV(PRIMARY)  > 25.)"
KstCuts += "& (INTREE( (ABSID=='K+') & (MIPCHI2DV(PRIMARY)  > 4.)"
KstCuts += "& (PT>350*MeV) & (P>1000*MeV) & (PIDK>-5.0) ))"
KstCuts += "& (INTREE((ABSID=='pi+') & (MIPCHI2DV(PRIMARY)  > 4.)"
KstCuts += "& (PT>350*MeV) & (P>1000*MeV)))"

######
# Bs #
######

Bs2KstKst = CombineParticles("Bs2KstKst")
Bs2KstKst.DecayDescriptor = " [B_s0 -> K*(892)0 K*(892)~0]cc"

Bs2KstKst.InputLocations = ["Phys/StdLooseDetachedKst2Kpi"]

Bs2KstKst.addTool( OfflineVertexFitter() )
Bs2KstKst.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bs2KstKst.OfflineVertexFitter.useResonanceVertex = False
Bs2KstKst.ReFitPVs = True
Bs2KstKst.DaughtersCuts = {"K*(892)0" : KstCuts,"K*(892)~0" : KstCuts }
                               
Bs2KstKst.CombinationCut ="ADAMASS('B_s0') < 500.*MeV"
Bs2KstKst.MotherCut ="(MIPCHI2DV(PRIMARY)<16) & (VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2>64) & (BPVVDSIGN > 0 )"




############################################
# Create StrippingLine with this selection #
############################################

line = StrippingLine('BsKst0Kst0Line'
               , prescale = 1
               , algos = [Bs2KstKst]
            )

