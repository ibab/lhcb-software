### @file DVTest-Bs2DsPi.py
 #
 #  Test file for HLT on selected Bs2DsPi events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2DsPi"
               , Decay = "{[[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^pi- {,gamma}{,gamma}]cc}"
               # V. Gligorov, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gligorov/OfflineDSTsForTriggerOptimization/Bs2DsPi_offsel_MC09_1k.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

