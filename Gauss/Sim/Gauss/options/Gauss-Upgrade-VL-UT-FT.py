## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Upgrade Detectors: VL UT FT Rich1Pmt Rich2Pmt
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade-VL-UT-FT.py <someInputJobConfiguration>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import LHCbApp
from Configurables import DDDBConf
from Configurables import CondDB

CondDB().Upgrade     = True
CondDB().AllLocalTagsByDataType=["VL+UT","FT"]
LHCbApp().DDDBtag    = "dddb-20121018"
LHCbApp().CondDBtag  = "simcond-20121001-vc-md100"
LHCbApp().Simulation = True

from Gauss.Configuration import *

Gauss().DetectorGeo  = { "Detectors": ['VL', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['VL', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['VL', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DataType     = "Upgrade" 
