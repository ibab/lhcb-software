### @file DVTest-Bs2JpsieePhi.py
 #
 #  Test file for HLT on selected Bs2JpsieePhi events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2JpsieePhi"
               , Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}) (J/psi(1S) -> ^e+ ^e- {,gamma}{,gamma})]cc"
               # 
               , Input = [  ])
CS.configure()

