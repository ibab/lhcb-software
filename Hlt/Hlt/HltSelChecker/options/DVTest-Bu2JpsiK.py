### @file DVTest-Bu2JpsiK.py
 #
 #  Test file for HLT on selectedBu2JpsiK  events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bu2JpsiK"
               , Decay = "[B+ -> ^K+ (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) {,gamma}{,gamma}]cc"
               # S. Poss, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/s/sposs/Bu2JPsiK_life_unb/Bu2JPsiK.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

