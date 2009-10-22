### @file DVTest-Bc2JpsiMuNu.py
 #
 #  Test file for HLT on selected Bc2JpsiMuNu events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bc2JpsiMuNu"
               , Decay = "[B_c+ -> (J/psi(1S) -> ^mu+ ^mu- {,gamma} {,gamma}) ^mu+ nu_mu {,gamma} {,gamma}]cc"
               # 
               , Input = [  ])
CS.configure()

