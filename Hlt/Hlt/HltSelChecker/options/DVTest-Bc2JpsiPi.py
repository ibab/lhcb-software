### @file DVTest-Bc2JpsiPi.py
 #
 #  Test file for HLT on selected Bc2JpsiPi events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bc2JpsiPi"
               , Decay = "[B_c+ -> (J/psi(1S) -> ^mu+ ^mu- {,gamma} {,gamma}) ^pi+ {,gamma} {,gamma}]cc"
               # 
               , Input = [  ])
CS.configure()

