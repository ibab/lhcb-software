### @file DVTest-PromptDstar.py
 #
 #  Test file for HLT on selected prompt D* events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="PromptDstar"
               , Decay = "{[D*(2010)+ -> (D0 -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^K- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc}"
               # Funai Xing, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/p/pxing/DST/DST_promptDstar2D0hhPiMC09.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

