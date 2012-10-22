## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Upgrade Detectors: Rich1Pmt Rich2Pmt
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade-Reference.py <someInputJobConfiguration>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import LHCbApp
from Configurables import DDDBConf
from Configurables import CondDB

CondDB().Upgrade     = True
LHCbApp().DDDBtag    = "dddb-20121018"
LHCbApp().CondDBtag  = "simcond-20121001-vc-md100"
LHCbApp().Simulation = True

from Gauss.Configuration import *

Gauss().DetectorGeo  = { "Detectors": ['Velo', 'PuVeto', 'TT', 'IT', 'OT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['Velo', 'PuVeto', 'TT', 'IT', 'OT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['Velo', 'PuVeto', 'TT', 'IT', 'OT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }

