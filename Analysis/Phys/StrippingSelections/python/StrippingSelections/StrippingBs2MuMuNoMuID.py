__author__ = 'Diego Martinez Santos'
__date__ = 'May 19, 2010'

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units



######
# Bs #
######

Bs2MuMuNoMuID = CombineParticles("StripBs2MuMuNoMuID")
Bs2MuMuNoMuID.DecayDescriptor = "B_s0 -> mu+ mu-"
Bs2MuMuNoMuID.InputLocations = ["Phys/StdNoPIDsMuons"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
Bs2MuMuNoMuID.addTool( OfflineVertexFitter() )
Bs2MuMuNoMuID.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bs2MuMuNoMuID.OfflineVertexFitter.useResonanceVertex = False
Bs2MuMuNoMuID.ReFitPVs = True
Bs2MuMuNoMuID.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)" ,
                                "mu-" : "(MIPCHI2DV(PRIMARY)> 25.)" }

Bs2MuMuNoMuID.CombinationCut = "(ADAMASS('B_s0')<600*MeV)";

Bs2MuMuNoMuID.MotherCut = "(VFASPF(VCHI2/VDOF)<9) & (MIPCHI2DV(PRIMARY)<25) & (BPVVDCHI2>225) & (BPVVDSIGN > 0 )"
                               

############################################
# Create StrippingLine with this selection #
############################################

line = StrippingLine('Bs2MuMuNoMuIDLine'
               , prescale = 1
               , algos = [Bs2MuMuNoMuID]
               )
