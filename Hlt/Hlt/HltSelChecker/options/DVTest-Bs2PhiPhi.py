### @file DVTest-Bs2PhiPhi.py
 #
 #  Test file for HLT on selected Bs2PhiPhi events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2PhiPhi"
               , Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma})(phi(1020) -> ^K+ ^K- {,gamma}{,gamma})]cc""
               # 
               , Input = [  ])
CS.configure()

