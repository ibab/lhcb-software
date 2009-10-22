### @file DVTest-Bs2KstarKstar.py
 #
 #  Test file for HLT on selected Bs2KstarKstar events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2KstarKstar"
               , Decay = "[B_s0 -> (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma}) ( K*(892)~0 -> ^pi+ ^K- {,gamma}{,gamma})]cc"
               # 
               , Input = [  ])
CS.configure()

