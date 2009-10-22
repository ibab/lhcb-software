### @file DVTest-SecondaryDstar.py
 #
 #  Test file for HLT on selected secondary D* events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="SecondaryDstar"
               , Decay = "{[D*(2010)+ -> (D0 -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^K- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc}"
               # 
               , Input = [  ])
CS.configure()

