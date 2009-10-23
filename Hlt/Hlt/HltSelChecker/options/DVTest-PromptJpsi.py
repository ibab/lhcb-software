### @file DVTest-PromptJpsi.py
 #
 #  Test file for HLT on selected Prompt J/psi events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-23
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="PromptJpsi"
               , Decay = "J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}"
               # Wenbin Qian, https://twiki.cern.ch/twiki/bin/view/LHCb/FlavourPhysics#MC09_samples_of_offline_selected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/w/wbqian/Jpsisamples/PromptJpsi.dst ' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

