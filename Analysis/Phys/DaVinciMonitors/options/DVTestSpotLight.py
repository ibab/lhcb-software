########################################################################
#
# Options for spotlight
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import SpotLight, PhysDesktop, CheckSelResult
from Configurables import LoKi__Hybrid__FilterCriterion as LoKiFilterCriterion

importOptions( "$DAVINCIROOT/options/DVOfficialStrippingFile.opts" )

ApplicationMgr().TopAlg += [ SpotLight() ]
SpotLight().addTool(PhysDesktop())

# Take the list from Global OR
globalor = CheckSelResult("GlobalOR")
SpotLight().Algorithms = globalor.Algorithms

SpotLight().addTool(LoKiFilterCriterion("MotherFilter"))
SpotLight().addTool(LoKiFilterCriterion("IntermediateFilter"))
SpotLight().addTool(LoKiFilterCriterion("FinalFilter"))

# @todo tighten !
SpotLight().FinalFilter.Code = "(PT>1*GeV) & (P>10*GeV)"
SpotLight().IntermediateFilter.Code = "(CHI2PDGM<4) & (BPVVDCHI2>16)"
SpotLight().MotherFilter.Code = "(CHI2PDGM<4) & (BPVIPCHI2()<100) & (BPVVDCHI2>16)"
#SpotLight().OutputLevel = 2
#SpotLight().PhysDesktop.OutputLevel = 3 

ApplicationMgr().EvtMax = 10000

# importOptions( "$DAVINCIROOT/options/DC06_stripped_bbincl_lumi2.opts" )

# some signal samples

EventSelector().Input   = [
#  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2Kpi-decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
##  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2DstMu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
#  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bs2psiphi-decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
#  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2MuMu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2D0Kst-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bd2MuMuKst-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2DsDs-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2Dspi-Decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2PhiPhi-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2KD-KSPiPi-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bu2Kee-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2Kmumu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Dst-PiD-HH-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

