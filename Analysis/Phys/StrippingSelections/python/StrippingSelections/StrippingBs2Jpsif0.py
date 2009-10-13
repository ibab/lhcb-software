# $Id: StrippingBs2Jpsif0.py,v 1.1 2009-10-13 13:24:16 poluekt Exp $

__author__ = 'Liming Zhang'
__date__ = '13/07/2009'
__version__ = '$Revision: 1.1 $'

'''
Bs->Jpsif0 stripping selection using LoKi::Hybrid and python
configurables. PV refitting is done. 
'''

from Gaudi.Configuration import *
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
import GaudiKernel.SystemOfUnits as Units


# Make the J/psi -> mu mu
# Filter the desktop
Jpsi2MuMu_forBs2Jpsif0 = FilterDesktop( "Jpsi2MuMu_forBs2Jpsif0")
Jpsi2MuMu_forBs2Jpsif0.InputLocations = ["StdLooseJpsi2MuMu"]
Jpsi2MuMu_forBs2Jpsif0.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 10.0)" \
                                 "& (MINTREE('mu+'==ABSID, PIDmu) > -10.0)" \
                                 "& (ADMASS('J/psi(1S)') < 50.*MeV)" \
                                 "& (VFASPF(VCHI2/VDOF) < 20.0)"

#f0 -> pi pi including same sign charge combination for background study
f02PiPi_forBs2Jpsif0 = CombineParticles("f02PiPi_forBs2Jpsif0")
f02PiPi_forBs2Jpsif0.DecayDescriptors = ["f_0(980) -> pi+ pi-", "f_0(980) -> pi- pi-", "f_0(980) -> pi+ pi+"]
f02PiPi_forBs2Jpsif0.InputLocations = ["StdLoosePions"]
f02PiPi_forBs2Jpsif0.CombinationCut = "ADAMASS('f_0(980)')<500*MeV"
f02PiPi_forBs2Jpsif0.DaughtersCuts = { "pi+" : " (MIPDV(PRIMARY)>0.05*mm) & (MIPCHI2DV(PRIMARY)>2.25) & (TRCHI2DOF<10) & (PIDK<10) & (PIDmu<10) " }
f02PiPi_forBs2Jpsif0.MotherCut = "(SUMTREE(PT, ABSID=='pi+')>900*MeV)"


######
# Bs #
######
Bs2Jpsif0 = CombineParticles("StripBs2Jpsif0")
Bs2Jpsif0.DecayDescriptor = "B_s0 -> J/psi(1S) f_0(980)"
Bs2Jpsif0.InputLocations = ["Jpsi2MuMu_forBs2Jpsif0",
                            "f02PiPi_forBs2Jpsif0"]
# Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi f0
Bs2Jpsif0.addTool( OfflineVertexFitter() )
Bs2Jpsif0.VertexFitters.update( { "" : "OfflineVertexFitter"} )
Bs2Jpsif0.OfflineVertexFitter.useResonanceVertex = False
Bs2Jpsif0.ReFitPVs = True

# Basic + loose offline PID
Bs2Jpsif0.CombinationCut = "ADAMASS('B_s0') < 250.*MeV"
Bs2Jpsif0.MotherCut = "(VFASPF(VCHI2/VDOF) < 6.) & (BPVDIRA>0.999) & (BPVIP()<0.08*mm) & (BPVVD>1.5*mm)"

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine('Bs2Jpsif0Line'
               , prescale = 1
               , algos = [Jpsi2MuMu_forBs2Jpsif0, f02PiPi_forBs2Jpsif0 , Bs2Jpsif0]
               )
