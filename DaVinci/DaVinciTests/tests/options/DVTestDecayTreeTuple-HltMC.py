 ## @file DVTestHltDecayTreeTuple-TisTos.py
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
 #

from Gaudi.Configuration import *

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

from Configurables import DecayTreeTuple
importOptions("$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").Inputs = ["Hlt2Bs2JpsiPhiDetachedBsCombine"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "B_s0 -> (^phi(1020) -> ^K+ ^K-) (^J/psi(1S) -> ^mu+ ^mu-)"
DaVinci().MoniSequence += [ DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().EvtMax = 100
