### @file DVTest-Bs2PhiGamma.py
 #
 #  Test file for HLT on selected Bs2JPhiGamma events
 #
 #  @author P. Koppenburg
 #  @date 2010-05-18
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2PhiGamma"
               , Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}) ^gamma {,gamma}{,gamma}]cc"
               # Fatima Soomro https://twiki.cern.ch/twiki/bin/viewauth/LHCb/RareDecaysTriggerTrainingMC09
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/f/fsoomro/MC09Signal/PhiGammaMC09.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

