### @file DVTest-Bs2PhiGamma.py
 #
 #  Test file for HLT on selected Bs2PhiGamma events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2PhiGamma"
               , Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}) gamma]cc"  # ignore photon
               # 
               , Input = [  ])
CS.configure()

