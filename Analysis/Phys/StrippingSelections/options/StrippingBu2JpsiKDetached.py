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

######
# Bu #
######

Bu2JpsiKDet = CombineParticles("StripBu2JpsiKDet")
Bu2JpsiKDet.DecayDescriptor = "[B+ -> J/psi(1S) K+]cc"
Bu2JpsiKDet.InputLocations = ["Phys/StdLooseJpsi2MuMu","Phys/StdNoPIDsKaons"]

Bu2JpsiKDet.addTool( OfflineVertexFitter() )
Bu2JpsiKDet.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bu2JpsiKDet.OfflineVertexFitter.useResonanceVertex = False
Bu2JpsiKDet.ReFitPVs = True
Bu2JpsiKDet.DaughtersCuts ={"J/psi(1S)" : JpsiCuts,
                          "K+": "(ISLONG) & (MIPCHI2DV(PRIMARY)>25)"} 
Bu2JpsiKDet.CombinationCut = "ADAMASS('B+') < 500.*MeV"
Bu2JpsiKDet.MotherCut = "(MIPCHI2DV(PRIMARY)<25)"

############################################
# Create StrippingLine with this selection #
############################################

Bu2JpsiKDetLine = StrippingLine('Bu2JpsiKDetLine'
               , prescale = 1
               , algos = [Bu2JpsiKDet]
               , stream = 'Bmuon'
               )
