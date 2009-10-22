### @file DVTest-Bu2D0K_KsPiPi_LL.py
 #
 #  Test file for HLT on selected Bu2D0K_KsPiPi_LL events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bu2D0K_KsPiPi_LL"
               , Decay = "[B+ -> (D~0 -> (KS0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc"
               # Susan Haines, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/s/shaines/MC09_OfflineSelEvts/B2DK_D2KsPiPi_KsLL_withBcand.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

