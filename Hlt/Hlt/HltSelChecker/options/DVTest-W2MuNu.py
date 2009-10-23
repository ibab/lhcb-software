### @file DVTest-W2MuNu.py
 #
 #  Test file for HLT on selected W2MuNu events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-23
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="W2MuNu"
               , Decay = "[W+ -> ^mu+ nu_mu {,gamma}{,gamma}]cc"
               # Sara Traynor, https://twiki.cern.ch/twiki/bin/view/LHCb/FlavourPhysics#MC09_samples_of_offline_selected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/t/traynor/WSelection.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()
