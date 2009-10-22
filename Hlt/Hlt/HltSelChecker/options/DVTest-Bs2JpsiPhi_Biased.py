### @file DVTest-Bs2JpsiPhi_Biased.py
 #
 #  Test file for HLT on selected Bs2JpsiPhi_Biased events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2JpsiPhi_Biased"
               , Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}) (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma})]cc"
               # Basem Khanji, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/b/bkhanji/MC09/BiasedBs2JpsiPhi_09.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

