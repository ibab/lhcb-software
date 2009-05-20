# $Id: StrippingBu2JpsiK.py,v 1.1 2009-05-20 15:41:24 gcowan Exp $

__author__ = 'Greig Cowan'
__date__ = '20/05/2009'
__version__ = '$Revision: 1.1 $'

'''
Bu->JpsiK stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. Based on roadmap selection
with loose PID cuts.
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units

######
# Bu #
######
Bu2JpsiK = CombineParticles("StripBu2JpsiK")
Bu2JpsiK.DecayDescriptor = "[ B+ -> J/psi(1S) K+ ]cc"
Bu2JpsiK.InputLocations = ["StdUnbiasedJpsi2MuMu",
                           "StdLooseKaons"]

Bu2JpsiK.addTool( OfflineVertexFitter() )
Bu2JpsiK.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bu2JpsiK.OfflineVertexFitter.useResonanceVertex = False
Bu2JpsiK.ReFitPVs = True

Bu2JpsiK.DaughtersCuts = {"K+" :
                          "  ((PIDK - PIDp)>-6.)"\
                          "& (PIDK > -5.)"\
                          "& (TRCHI2DOF<4.)"\
                          "& (PT>1300.*MeV)"\
                          "& (P>10000.*MeV)"
                          }

Bu2JpsiK.CombinationCut = "ADAMASS('B+') < 300.*MeV"
Bu2JpsiK.MotherCut = "(VFASPF(VCHI2/VDOF)<5.)"

############################################
# Create StrippingLine with this selection #
############################################
Bu2JpsiKLine = StrippingLine('Bu2JpsiKLine'
               , prescale = 1
               , algos = [Bu2JpsiK]
               , stream = 'BExclusive'
               )
