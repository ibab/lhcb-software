# $Id: StrippingBs2PhiPhi.py,v 1.2 2009-09-04 16:02:55 poluekt Exp $

__author__ = 'Sebastian Schleich'
__date__ = '06/08/2009'
__version__ = '$Revision: 1.2 $'

'''
Bs->PhiPhi stripping selection
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter, PhysDesktop
import GaudiKernel.SystemOfUnits as Units


StrippingPhi2KK = StrippingMember ( FilterDesktop
                   , "StrippingPhi2KK"
                   , InputLocations = ["StdLooseDetachedPhi2KK"]
                   , Code = "(ADMASS('phi(1020)') < 20.*MeV)" 
                   )

BsCombine = StrippingMember( CombineParticles
          , "BsCombine"
          , DecayDescriptor =  "[B_s0 -> phi(1020) phi(1020)]cc"
          , InputLocations  = [ "%StrippingPhi2KK" ]
          , CombinationCut  = "(ADAMASS('B_s0')<300*MeV)&(ACHILD(PT,1)*ACHILD(PT,2)>3*GeV*GeV)"
          , MotherCut       = "(VFASPF(VCHI2)<40)"
          )

Bs2PhiPhiLine = StrippingLine('Bs2PhiPhiLine'
               , prescale = 1 
               , postscale = 1
               , algos = [StrippingPhi2KK, BsCombine]
               , stream = 'Hadron'
               )

Bs2PhiPhiLineWide = Bs2PhiPhiLine.clone('Bs2PhiPhiLineWide'
               , prescale = 0.1 
               , postscale = 1
               , FilterDesktopStrippingPhi2KK = {"Code": "MM < 1100.*MeV"}
               )

