## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Upgrade Detectors: VL UT FT Rich1Pmt Rich2Pmt
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade-VL-UT-FT.py <someInputJobConfiguration>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import CondDB

CondDB().Upgrade     = True
CondDB().AllLocalTagsByDataType=["VL+UT","FT"]

from Configurables import Gauss

Gauss().DetectorGeo  = { "Detectors": ['VL', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['VL', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['VL', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DataType     = "Upgrade" 
