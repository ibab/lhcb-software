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

spot = SpotLight()
spot.addTool(PhysDesktop)
# @todo Get thet from python
spot.Algorithms = [ "PreselB2DiMuon",
  "PreselBd2KstarMuMu",
  "PreselBu2KMuMu",
  "PreselBd2Kstaree",
  "PreselBs2MuMu",
  "PreselBs2GammaMuMu",
  "PreselB2EMu",
  "PreselBu2LLK",
  "PreselBs2PhiEtac",
  "PreselHeavyDimuon",
  "PreselJpsi2ee",
  "PreselUnbiasedJpsi2ee",
  "PreselXb2GammaX",
  "PreselB2HH",
  "PreselBd2KsPiPi",
  "PreselBd2PhiKs",
  "PreselBs2PhiPhi",
  "PreselLambdaB2LambdaPhi",
  "PreselLambdaB2LambdaRho",
  "PreselLambdaB2LambdaOmega",
  "PreselBs2DsDs",
  "PreselBs2DsH",
  "PreselBs2DsMuX",
  "PreselBs2DsRho",
  "PreselBd2DstarMu",
  "PreselBd2DstarPi",
  "PreselDstar2D0Pi_D02HH",
  "PreselDstar2D0Pi_D02HHHH",
  "PreselDstarNoPID",
  "PreselBd2D0Kst_D02HH",
  "PreselBd2D0Kst_D02KsKK",
  "PreselBd2D0Kst_D02KsPiPi",
  #  "PreselBu2D0K_D02HH",
  "PreselBu2D0K_D02HHHH",
  "PreselBu2D0K_D02KMuNu",
  "PreselBu2D0K_D02KsKK",
  "PreselBu2D0K_D02KsPiPi",
  "PreselBu2D0Mu",
  #  "PreselBu2D0Pi",
  "PreselBu2D0Tau2PiPiPi",
  "PreselBd2DPi",
  "PreselHWZ2bbl",
  "PreselBd2KPiPi0",
  "PreselBd2PiPiPi0",
  "PreselBd2Rho0Rho0",
  "PreselBu2HHH",
  "PreselBu2D0K_D02KPiPi0",
  "PreselBu2D0H_D02HH",
  "PreselBu2D0K_D02KsKPi" ]

spot.addTool(LoKiFilterCriterion("MotherFilter"))
spot.addTool(LoKiFilterCriterion("IntermediateFilter"))
spot.addTool(LoKiFilterCriterion("FinalFilter"))

# @todo tighten !
spot.FinalFilter.Code = "(PT>1*GeV) & (P>10*GeV)"
spot.IntermediateFilter.Code = "(CHI2PDGM<4) & (BPVVDCHI2>16)"
spot.MotherFilter.Code = "(CHI2PDGM<4) & (BPVIPCHI2()<100) & (BPVVDCHI2>16)"
########################################################################
#
from Configurables import DaVinci
DaVinci().MoniSequence += [ spot ]
DaVinci().HistogramFile = "DVMonitors.root"
##############################################################################
#
# Most of this will be configured from Dirac
#
##############################################################################
DaVinci().EvtMax = 1000
DaVinci().DataType = "DC06" # Default is "DC06"
DaVinci().Simulation = True
DaVinci().MainOptions = "$STRIPPINGROOT/options/Presel.opts"
DaVinci().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2Kpi-decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2DstMu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bs2psiphi-decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2MuMu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2D0Kst-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bd2MuMuKst-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2DsDs-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2Dspi-Decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bs2PhiPhi-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2KD-KSPiPi-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bu2Kee-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2Kmumu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ,
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Dst-PiD-HH-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

