### @file DVTest-Bd2KstarGamma.py
 #
 #  Test file for HLT on selected Bs2PhiGamma events
 #
 #  @author P. Koppenburg
 #  @date 2010-05-18
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2KstarGamma"
               , Decay = "[B0 -> (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma}) ^gamma {,gamma}{,gamma}]cc"
               # Fatima Soomro https://twiki.cern.ch/twiki/bin/viewauth/LHCb/RareDecaysTriggerTrainingMC09
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/f/fsoomro/MC09Signal/KstGammaMC09.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

