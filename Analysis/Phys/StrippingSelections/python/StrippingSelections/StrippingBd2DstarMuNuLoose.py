# $Id: StrippingBd2DstarMuNuLoose.py,v 1.1 2010-01-18 10:15:48 gcowan Exp $

__author__ = 'Greig Cowan, Marta Calvi'
__date__ = '10/12/2009'
__version__ = '$Revision: 1.1 $'

'''
Bd->Dstar mu nu loose stripping selection using LoKi::Hybrid and python
configurables.
'''
from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from Configurables import CombineParticles
import GaudiKernel.SystemOfUnits as Units

Bd2DstarMu = CombineParticles("Bd2DstarMuLoose")
Bd2DstarMu.DecayDescriptor = "[B0 -> D*(2010)- mu+]cc"
Bd2DstarMu.InputLocations = ["StdVeryLooseDstarWithD02KPi", "StdLooseMuons"]

# D* has the following decay chain:  D*+ -> ( D0 -> K pi ) pi 
muonCuts = "  (ISLONG)"\
           "& (PT > 800.*MeV)"\
           "& (MIPDV(PRIMARY)>0.04*mm)"\
           "& (TRCHI2DOF < 10.)"

DstarCutsD0 = "CHILDCUT("\
             "    (PT > 1600.*MeV)"\
             ",1)"

DstarCutsK = "& CHILDCUT("\
            "   CHILDCUT("\
            "       (ISLONG)"\
            "     & (PT > 350.*MeV)"\
            "   ,1)"\
            ",1)"

DstarCutsPi= "& CHILDCUT("\
             "   CHILDCUT("\
             "       (ISLONG)"\
             "     & (PT > 350.*MeV)"\
             "   ,2)"\
             ",1)"

finalDstarCuts = DstarCutsD0 + DstarCutsK + DstarCutsPi 

Bd2DstarMu.DaughtersCuts = { "mu-" : muonCuts,
                             "D*(2010)+" : finalDstarCuts}

Bd2DstarMu.CombinationCut = "  (DAMASS('B0') > -2279.*MeV)"\
                            "& (DAMASS('B0') < 0*MeV)"

Bd2DstarMu.MotherCut = "  (VFASPF(VCHI2) < 20.)"


line = StrippingLine('Bd2DstarMuNuLooseLine'
               , prescale = 1
               , algos = [Bd2DstarMu]
               )
