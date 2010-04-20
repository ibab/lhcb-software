# $Id: StrippingBd2DstarMuNu.py,v 1.2 2010-04-20 23:02:13 gcowan Exp $

__author__ = 'Greig Cowan, Marta Calvi'
__date__ = '30/07/2009'
__version__ = '$Revision: 1.2 $'

'''
Bd->Dstar mu nu stripping selection using LoKi::Hybrid and python
configurables.
'''
from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from Configurables import CombineParticles
import GaudiKernel.SystemOfUnits as Units

Bd2DstarMu = CombineParticles("Bd2DstarMu")
Bd2DstarMu.DecayDescriptor = "[B0 -> D*(2010)- mu+]cc"
Bd2DstarMu.InputLocations = ["StdLooseDstarWithD02KPi", "StdLooseMuons"]

# StdLoose D* has the following decay chain:  D*+ -> pi ( D0 -> K pi ) 
muonCuts = "  (ISLONG)"\
           "& (PT > 800.*MeV)"\
           "& (PIDmu > -5.)"\
           "& (MIPCHI2DV(PRIMARY) > 4.)"\
           "& (TRCHI2DOF < 15.)"

# Cuts on D*
DstarCuts1 = "  (ADMASS('D*(2010)+') < 30.*MeV)"\
             "& (PT > 2000.*MeV)"\
             "& (VFASPF(VCHI2) < 20. )"\
             "& (M-MAXTREE('D0'==ABSID,M) < 165.*MeV)"

# Cuts on D0 from D*
DstarCuts2 = "& CHILDCUT("\
             "    (PT > 1800.*MeV)"\
             "  & (ADMASS('D0') < 30.*MeV )"\
             "  & (BPVVDCHI2 > 16.)"\
             "  & (VFASPF(VCHI2) < 20. )"\
             ",2)"

# Cuts on pi from D*
DstarCuts3 = "& CHILDCUT("\
             "    (PT > 110.*MeV)"\
             "  & (TRCHI2DOF < 15.)"\
             "  & (MIPCHI2DV(PRIMARY) > 1.)"\
             ",1)"
               
# Cuts on K from D0
DstarCuts4 = "& CHILDCUT("\
            "   CHILDCUT("\
            "       (ISLONG)"\
            "     & (P > 2.*GeV)"\
            "     & (PT > 350.*MeV)"\
            "     & (PIDK > -5.)"\
            "     & (TRCHI2DOF < 20.)"\
            "     & (MIPCHI2DV(PRIMARY) > 4.)"\
            "   ,1)"\
            ",2)"

# Cuts on pi from D0
DstarCuts5 = "& CHILDCUT("\
             "   CHILDCUT("\
             "       (ISLONG)"\
             "     & (P > 2.*GeV)"\
             "     & (PT > 300.*MeV)"\
             "     & (TRCHI2DOF < 20.)"\
             "     & (MIPCHI2DV(PRIMARY) > 4.)"\
             "   ,2)"\
             ",2)"

finalDstarCuts = DstarCuts1 + DstarCuts2 + DstarCuts3 + DstarCuts4 + DstarCuts5

Bd2DstarMu.DaughtersCuts = { "mu-" : muonCuts,
                             "D*(2010)+" : finalDstarCuts}

Bd2DstarMu.CombinationCut = "  (DAMASS('B0') > -2279.*MeV)"\
                            "& (DAMASS('B0') < -59.*MeV)"

Bd2DstarMu.MotherCut = "  (VFASPF(VCHI2) < 15.)"\
                       "& (BPVVDZ > 0.5)"\
                       "& (MINTREE(ABSID=='D0', VFASPF(VZ))-VFASPF(VZ) > -2.5 )"

line = StrippingLine('Bd2DstarMuNuLine'
               , prescale = 1
               , algos = [Bd2DstarMu]
               )
