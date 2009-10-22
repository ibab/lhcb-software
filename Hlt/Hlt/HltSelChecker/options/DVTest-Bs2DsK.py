### @file DVTest-Bs2DsK.py
 #
 #  Test file for HLT on selected Bs2DsK events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bs2DsK"
               , Decay = "{[[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^K- {,gamma}{,gamma}]cc}"
                 # V. Gligorov, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gligorov/OfflineDSTsForTriggerOptimization/Bs2DsK_offsel_MC09_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                         , "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gligorov/OfflineDSTsForTriggerOptimization/Bs2DsK_offsel_MC09_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                         , "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gligorov/OfflineDSTsForTriggerOptimization/Bs2DsK_offsel_MC09_3.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
                         , "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gligorov/OfflineDSTsForTriggerOptimization/Bs2DsK_offsel_MC09_4.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                           ])
CS.configure()

