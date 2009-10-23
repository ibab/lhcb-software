### @file DVTest-Z0.py
 #
 #  Test file for HLT on selected Z0 events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-23
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Z0"
               , Decay = "Z0 -> ^mu+ ^mu- {,gamma}{,gamma}"
               # James Keaveney , https://twiki.cern.ch/twiki/bin/view/LHCb/FlavourPhysics#MC09_samples_of_offline_selected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/k/keaveney/MC09/Z/Z02MuMuMC09_SELECTED.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()
