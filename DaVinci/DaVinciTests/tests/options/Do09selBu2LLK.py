"""
@file Do09selBu2LLK.py
   Configuration options for the execution of the selection of
   B+ -> l+ l- K+ 
 
   @author P. Koppenburg
   @date 2009-01-09
"""
from Gaudi.Configuration import *
##########################################################################
#
# presel cuts
#
leptoncut = "(PT>800*MeV)" 
kaoncut   = "(PT>1400*MeV) & (MIPCHI2DV(PRIMARY)>4)"
llcut     = "(VFASPF(VCHI2/VDOF)<9) & (MINTREE((ABSID<20),PT)>1*GeV)"
combcut   = "(ADAMASS('B0')<600*MeV)"
bcut      = "((VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<25) & (BPVDIRA>0.999) & (BPVVDCHI2>25))"
##########################################################################
#
# hard cuts
#
hardkaon     = " (MINTREE(ABSID=='K+',PT)>1400) "
hardelectron = " ( 2 == NINTREE( (ABSID=='e-') & (PT>1000) & (PIDe>6) & (MIPCHI2DV(PRIMARY)>4) )) "
hardmuon     = " ( 2 == NINTREE( (ABSID=='mu-') & (PT>1000) & (MIPCHI2DV(PRIMARY)>4) )) "
harddilepton = " (INTREE( (ID=='J/psi(1S)') & (MM<2449) & (BPVVDCHI2>25) )) "
hardB        = " (VFASPF(VCHI2/VDOF)<6.67) & (BPVVDCHI2>100) & (BPVIPCHI2()<25)  & (BPVDIRA>0.9999) " 
###########################################################################
#
# Config
#
from Configurables import GaudiSequencer
Sel09Bu2LLKFilterSequence = GaudiSequencer("Sel09Bu2LLKFilterSequence")
##########################################################################
from Configurables import CombineParticles
Bu2LLK09_DiLepton = CombineParticles("Bu2LLK09_DiLepton")
Sel09Bu2LLKFilterSequence.Members += [ Bu2LLK09_DiLepton ]

Bu2LLK09_DiLepton.Inputs = [ "Phys/StdLooseElectrons", "Phys/StdLooseMuons" ]
Bu2LLK09_DiLepton.DecayDescriptors = [ "J/psi(1S) -> e+ e-", "J/psi(1S) -> mu+ mu-" ]

Bu2LLK09_DiLepton.DaughtersCuts = {"e+" : leptoncut, "mu+" : leptoncut }
Bu2LLK09_DiLepton.CombinationCut = "(AM<6*GeV)"
Bu2LLK09_DiLepton.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"
###########################################################################
# Preselection
#
Presel09Bu2LLK = CombineParticles("Presel09Bu2LLK")
Sel09Bu2LLKFilterSequence.Members += [ Presel09Bu2LLK ]
Presel09Bu2LLK.Inputs  = [ "Phys/Bu2LLK09_DiLepton",
                           "Phys/StdLooseKaons"]
Presel09Bu2LLK.DecayDescriptor = "[ B+ -> J/psi(1S) K+ ]cc"
Presel09Bu2LLK.DaughtersCuts = { "K+"        : kaoncut ,
                              "J/psi(1S)" : llcut }
Presel09Bu2LLK.CombinationCut = combcut
Presel09Bu2LLK.MotherCut = bcut
##########################################################################
# Selection
#
from Configurables import FilterDesktop
Sel09Bu2LLK = FilterDesktop("Sel09Bu2LLK")
Sel09Bu2LLK.Inputs  = ["Phys/Presel09Bu2LLK"]
Sel09Bu2LLK.Code = hardkaon+" & ("+hardelectron+" | "+hardmuon+" ) & "+harddilepton+" & "+hardB
Sel09Bu2LLKFilterSequence.Members += [ Sel09Bu2LLK ]

##########################################################################
#ApplicationMgr().TopAlg += [ Sel09Bu2LLKFilterSequence ]
