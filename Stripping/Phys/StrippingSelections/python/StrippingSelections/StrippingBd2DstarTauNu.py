# $Id: StrippingBd2DstarTauNu.py,v 1.4 2010-09-05 22:01:54 gcowan Exp $

__author__ = 'Greig Cowan, Marta Calvi, Stefania Vecchi - adjusted for Tau by Anne Keune'
__date__ = '29/09/2010'
__version__ = '$Revision: 0.0 $'

'''
Bd->Dstar tau nu stripping selection using three CombineParticles algorithms with LoKi Cuts.
'''
from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine
from Configurables import CombineParticles, FilterDesktop
import GaudiKernel.SystemOfUnits as Units

# StdLoose D* has the following decay chain:  D*+ -> pi ( D0 -> K pi )
# Cuts on D*
DstarCuts1 = " (VFASPF(VCHI2/VDOF) < 20. )"
             
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

####

FilteredDstars = FilterDesktop("FilteredDstars")
FilteredDstars.InputLocations = ["Phys/StdLooseDstarWithD02KPi"]
FilteredDstars.Code = finalDstarCuts

####

FilteredPis = FilterDesktop("FilteredPis")
FilteredPis.InputLocations = ["Phys/StdLoosePions"]
FilteredPis.Code = "(TRCHI2DOF<4.) & (PT>150.) & (PIDpi>-3.) & (P>1.2*GeV)  & (MIPDV(PRIMARY)>0.04) & (BPVIPCHI2()>6)"

####

X2PiPi = CombineParticles("X2PiPi")
X2PiPi.InputLocations = ["FilteredPis"]
X2PiPi.DecayDescriptors = ["[rho(770)0 -> pi+ pi-]cc","[rho(770)+ -> pi+ pi+]cc"]
X2PiPi.MotherCut = "(PT>600*MeV) & (VFASPF(VCHI2/VDOF)<9) & in_range(750,M,1500) & (BPVVDCHI2 > 50.) & (BPVDIRA > 0.98) &  (BPVIPCHI2()>7)"
X2PiPi.CombinationCut = "(APT1+APT2>800*MeV) & ((ACHILD(MIPDV(PRIMARY),1)+ACHILD(MIPDV(PRIMARY),2))>0.2) & (AMAXCHILD('pi-'==ABSID,BPVIPCHI2())>10) & (AMAXCHILD('pi-'==ABSID,PT)>500)"

####

Tau2PiPiPi = CombineParticles("Tau2PiPiPi")
Tau2PiPiPi.InputLocations = ["FilteredPis","X2PiPi"]
Tau2PiPiPi.DecayDescriptors = ["[tau+ -> rho(770)0 pi+]cc","[tau+ -> rho(770)+ pi-]cc"]
Tau2PiPiPi.MotherCut = "(PT>800) & (P>9.*GeV) & in_range(1000,M,1780) & (VFASPF(VCHI2/VDOF)<9) & (BPVIPCHI2()>7) & (MIPDV(PRIMARY)>0.03) & (BPVVDCHI2 > 50.) & (BPVDIRA > 0.98)"
Tau2PiPiPi.CombinationCut = "(AMAXCHILD(MAXTREE('pi-'==ABSID,MIPDV(PRIMARY)))>0.15)"

####

Bd2DstarTau = CombineParticles("Bd2DstarTau")
Bd2DstarTau.InputLocations = ["FilteredDstars","Tau2PiPiPi"]
Bd2DstarTau.DecayDescriptors = ["[B0 -> D*(2010)- tau+]cc ", "[B0 -> D*(2010)+ tau+]cc "] # include wrong sign
Bd2DstarTau.DaughtersCuts = {"D*(2010)+" : finalDstarCuts}

Bd2DstarTau.CombinationCut = "  (DAMASS('B0') > -2279.*MeV)"\
                             "& (DAMASS('B0') < -579.*MeV)"

Bd2DstarTau.MotherCut = "  (VFASPF(VCHI2/VDOF) > 0)"\
                        "& (BPVDIRA > 0.98)"\
                        "& (D2DVVD(2)-D2DVVD(1)>0)"

Seq = GaudiSequencer("Seq")
Seq.Members = [ FilteredDstars ]
Seq.Members += [ FilteredPis ]
Seq.Members += [ X2PiPi ]
Seq.Members += [ Tau2PiPiPi ]
Seq.Members += [ Bd2DstarTau ]

line = StrippingLine('Bd2DstarTauNuLine'
                     , prescale = 1
                     , algos = [Seq]
                     )
