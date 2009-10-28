### @file DVTest-Bd2Jpsif0.py
 #
 #  Test file for HLT on selected Bd2Jpsif0 events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2Jpsif0"
               , Decay = "[B_s0  -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (f_0(980) -> ^pi+ ^pi- {,gamma}{,gamma})]cc"
               # Liming Zhang , https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/l/lzhang/MC09/Bs2Jpsif0.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

