### @file DVTest-SecondaryPsi2S.py
 #
 #  Test file for HLT on selected Secondary Psi(2S) events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-23
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="SecondaryPsi2S"
               , Decay = "psi(2S) -> ^mu+ ^mu- {,gamma}{,gamma}"
               # Giovanni Sabatino, https://twiki.cern.ch/twiki/bin/view/LHCb/FlavourPhysics#MC09_samples_of_offline_selected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/s/sabatino/168/outputdata/psi_2S.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()
