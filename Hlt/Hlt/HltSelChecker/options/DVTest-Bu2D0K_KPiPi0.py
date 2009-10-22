### @file DVTest-Bu2D0K_KPiPi0.py
 #
 #  Test file for HLT on selected Bu2D0K_KPiPi0 events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bu2D0K_KPiPi0"
               , Decay = "[B+ -> (D~0 -> (pi0 -> gamma gamma) ^pi+ ^pi- {,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc"
               # Andrea Contu, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/a/acontu/SelBu2D0K_D02KPiPi0/DSTs/SelBu2D0K_D02KPiPi0_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

