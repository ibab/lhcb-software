### @file DVTest-Bs2JpsiPhi_Unbiased.py
 #
 #  Test file for HLT on selected Bs2JpsiPhi_Unbiased events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2JpsiPhi_Unbiased"
               , Decay = "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}) (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma})]cc"
               # Basem Khanji, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/b/bkhanji/MC09/UnBiasedBs2JpsiPhi_09.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

from Configurables import DecayTreeTuple
importOptions("$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2Bs2JpsiPhiDetachedBsCombine"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "B_s0 -> (^phi(1020) -> ^K+ ^K-) (^J/psi(1S) -> ^mu+ ^mu-)"
DaVinci().MoniSequence += [ DecayTreeTuple("Hlt2DecayTreeTuple") ]
