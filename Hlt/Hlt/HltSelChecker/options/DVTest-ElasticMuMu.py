### @file DVTest-ElasticMuMu.py
 #
 #  Test file for HLT on selected elestic mumu events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-23
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="ElasticMuMu"
               , Decay = ""
               # Dermot Moran, https://twiki.cern.ch/twiki/bin/view/LHCb/FlavourPhysics#MC09_samples_of_offline_selected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dmoran/ElasticMuMu/ElasticMuMu_SEL.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

