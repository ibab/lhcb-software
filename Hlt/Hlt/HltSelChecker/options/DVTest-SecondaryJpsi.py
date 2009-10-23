### @file DVTest-SecondaryJpsi.py
 #
 #  Test file for HLT on selected Secondary J/psi events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-23
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="SecondaryJpsi"
               , Decay = "J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}"
               # Wenbin Qian, https://twiki.cern.ch/twiki/bin/view/LHCb/FlavourPhysics#MC09_samples_of_offline_selected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/w/wbqian/Jpsisamples/JpsiFromb.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()
