# $Id: StrippingBd2DstarMuNu.py,v 1.4 2010-09-05 22:01:54 gcowan Exp $

__author__ = 'Greig Cowan, Marta Calvi, Stefania Vecchi'
__date__ = '03/09/2010'
__version__ = '$Revision: 1.4 $'

'''
Bd->Dstar mu nu stripping selection using LoKi::Hybrid and python
configurables.
'''
from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from Configurables import CombineParticles
import GaudiKernel.SystemOfUnits as Units
from PhysSelPython.Wrappers import Selection, DataOnDemand

_StdLooseDstarWithD02KPi = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi")
_StdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons")

Bd2DstarMu = CombineParticles("Bd2DstarMu")
Bd2DstarMu.DecayDescriptors = ["[B0 -> D*(2010)- mu+]cc ", "[B0 -> D*(2010)+ mu+]cc "] # include wrong sign

# StdLoose D* has the following decay chain:  D*+ -> pi ( D0 -> K pi ) 
muonCuts = "  (ISLONG)"\
           "& (PT > 800.*MeV)"\
           "& (PIDmu > -5.)"\
           "& (MIPCHI2DV(PRIMARY) > 4.)"\
           "& (TRCHI2DOF < 10.)"

# Cuts on D*
DstarCuts1 = " (VFASPF(VCHI2/VDOF) < 20. )"#\  # is <25 in the StdLooseDstarWithD02KPi
             

# Cuts on D0 from D*
DstarCuts2 = "& CHILDCUT("\
             "    (PT > 1600.*MeV)"\
             "  & (ADMASS('D0') < 40.*MeV )"\
             "  & (BPVVDCHI2 > 50.)"\
             ",2)"

# Cuts on pi from D*
DstarCuts3 = "& CHILDCUT("\
             "    (PT > 110.*MeV)"\
             "  & (TRCHI2DOF < 10.)"\
             "  & (MIPDV(PRIMARY) > 0.04*mm)"\
             ",1)"

# Cuts on K from D0
DstarCuts4 = "& CHILDCUT("\
            "   CHILDCUT("\
            "       (ISLONG)"\
            "     & (P > 2.*GeV)"\
            "     & (PT > 350.*MeV)"\
            "     & (PIDK > -5.)"\
            "   ,1)"\
            ",2)"

# Cuts on pi from D0
DstarCuts5 = "& CHILDCUT("\
             "   CHILDCUT("\
             "       (ISLONG)"\
             "     & (P > 2.*GeV)"\
             "     & (PT > 350.*MeV)"\
             "   ,2)"\
             ",2)"

finalDstarCuts = DstarCuts1 + DstarCuts2 + DstarCuts3 + DstarCuts4 + DstarCuts5

Bd2DstarMu.DaughtersCuts = { "mu-" : muonCuts,
                             "D*(2010)+" : finalDstarCuts}

Bd2DstarMu.CombinationCut = "  (DAMASS('B0') > -2279.*MeV)"\
                            "& (DAMASS('B0') < -59.*MeV)"

Bd2DstarMu.MotherCut = "  (VFASPF(VCHI2/VDOF) < 10.)"\
                       "& (BPVDIRA > 0.999)"\
                       "& (BPVVDZ > 0.5)"\
                       "& (MINTREE(ABSID=='D0', VFASPF(VZ))-VFASPF(VZ) > -2.5 )"


# Using Selection framework 
SelBd2DstarMu = Selection("SelBd2DstarMu",
                          Algorithm = Bd2DstarMu,
                          RequiredSelections = [ _StdLooseDstarWithD02KPi, _StdLooseMuons])

line = StrippingLine('Bd2DstarMuNuLine'
                     , prescale = 1
                     , algos = [ SelBd2DstarMu ]
                     )
