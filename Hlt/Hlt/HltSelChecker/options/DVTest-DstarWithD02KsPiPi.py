### @file DVTest-DstarWithD02KsPiPi.py
 #
 #  Test file for HLT on selected DstarWithD02KsPiPi events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="DstarWithD02KsPiPi"
               , Decay = "{[D*(2010)+ -> (D0 -> ( KS0 -> ^pi+ ^pi- {,gamma}{,gamma} ) ^pi+ ^pi- {,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc}"
               # Mat Charles, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mcharles/121/outputdata/dstar.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

