### @file DVTest-Bd2eeKstar.py
 #
 #  Test file for HLT on selected Bd2eeKstar events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2eeKstar"
               , Decay = "{[[B0]nos => ^K+ ^pi- ^e+ ^e- {,gamma}{,gamma}{,gamma}{,gamma}]cc, [[B0]os => ^K- ^pi+ ^e+ ^e- {,gamma}{,gamma}{,gamma}{,gamma}]cc}"
               # 
               , Input = [  ])
CS.configure()

