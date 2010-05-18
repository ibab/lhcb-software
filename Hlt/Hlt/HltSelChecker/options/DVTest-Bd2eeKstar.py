### @file DVTest-Bd2Kstaree.py
 #
 #  Test file for HLT on selected events
 #
 #  @author P. Koppenburg
 #  @date 2010-05-18
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2Kstaree"
               , Decay = "[B0 -> (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma}) ^e+ e^- {,gamma}{,gamma}]cc"
               # Fatima Soomro https://twiki.cern.ch/twiki/bin/viewauth/LHCb/RareDecaysTriggerTrainingMC09
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mschune/MC09_offline_Bd2eeKstar.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

