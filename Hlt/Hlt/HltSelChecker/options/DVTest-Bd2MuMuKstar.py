### @file DVTest-Bd2MuMuKstar.py
 #
 #  Test file for HLT on selected Bd2MuMuKstar events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2MuMuKstar"
               , Decay = "{[[B0]nos => ^K+ ^pi- ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc, [[B0]os => ^K- ^pi+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc}"
               # Mitesh Patel, e-mail
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/0/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/1/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/2/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/3/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/4/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/5/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/6/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/7/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/8/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'",
                           "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mitesh/258/9/outputdata/KStarMuMu_forTrigger.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()

