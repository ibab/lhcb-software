__author__ = 'Diego Martinez Santos'
__date__ = 'June 11, 2010'

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

PhiCuts = "(MIPCHI2DV(PRIMARY)> 25) & (ADMASS('phi(1020)')<10*MeV)"
#PhiCuts += "& (INTREE( (ID=='K+') & (MIPCHI2DV(PRIMARY)  > 4.))) "
#PhiCuts += "& (INTREE( (ID=='K-') & (MIPCHI2DV(PRIMARY)  > 4.)))"

######
# Bs #
######

Bs2JpsiPhiForBs2MuMu = CombineParticles("StripBs2JpsiPhiForBs2MuMu")
Bs2JpsiPhiForBs2MuMu.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
Bs2JpsiPhiForBs2MuMu.InputLocations = ["Phys/StdLooseJpsi2MuMu", "Phys/StdLooseDetachedPhi2KK"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
Bs2JpsiPhiForBs2MuMu.addTool( OfflineVertexFitter() )
Bs2JpsiPhiForBs2MuMu.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bs2JpsiPhiForBs2MuMu.OfflineVertexFitter.useResonanceVertex = False
Bs2JpsiPhiForBs2MuMu.ReFitPVs = True
Bs2JpsiPhiForBs2MuMu.DaughtersCuts = {"J/psi(1S)" : JpsiCuts, "phi(1020)" : PhiCuts }
                               
Bs2JpsiPhiForBs2MuMu.CombinationCut = "ADAMASS('B_s0') < 500.*MeV"
Bs2JpsiPhiForBs2MuMu.MotherCut = "(MIPCHI2DV(PRIMARY)<25)"

############################################
# Create StrippingLine with this selection #
############################################

line = StrippingLine('Bs2JpsiPhiForBs2MuMuLine'
               , prescale = 1
               , algos = [Bs2JpsiPhiForBs2MuMu]
               )
