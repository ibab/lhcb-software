### @file DVTest-Bd2JpsiKstar.py
 #
 #  Test file for HLT on selected Bd2JpsiKstar events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2JpsiKstar"
               , Decay = "[B0 -> (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma}) (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) {,gamma}{,gamma}]cc"
               # S. Poss, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/s/sposs/Bd2JPsiKst_life_unb/Bd2JPsiKst.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

