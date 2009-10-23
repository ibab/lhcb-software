### @file DVTest-DrellYan2GeV.py
 #
 #  Test file for HLT on selected Drell-Yan events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-23
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="DrellYan10GeV"
               , Decay = "pp => ^mu+ ^mu-"
               # Jonathan Anderson, https://twiki.cern.ch/twiki/bin/view/LHCb/FlavourPhysics#MC09_samples_of_offline_selected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/j/janderso/DY5GeV_MC09_Selected.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

