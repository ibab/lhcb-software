### @file DVTest-Bu2eeK.py
 #
 #  Test file for HLT on selected  events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
input = []
for i in range(39):
    input.append( "DATAFILE='PFN:castor:/castor/cern.ch/user/p/pkoppenb/MC09-Bu2eeK/Bu2LLK-"+str(i)+".dst' TYP='POOL_ROOTTREE' OPT='READ'" )

CS = CheckerSeq( DV=DaVinci()
               , Signal="Bu2eeK"
               , Decay = "[B+ -> ^K+ ^e+ ^e- {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}]cc"
               # P. Koppenburg, hey that's me!
               , Input = input )
CS.configure()

