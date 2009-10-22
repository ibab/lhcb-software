### @file DVTest-Bs2MuMu.py
 #
 #  Test file for HLT on selected Bs2MuMu events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2MuMu"
               , Decay = "[B_s0 -> ^mu+ ^mu- {,gamma}{,gamma}]cc"
               # 
               , Input = [  ])
CS.configure()

