## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Upgrade Detectors: VP UT FT Rich2Pmt
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade-VP-UT-FT-noRich1.py <someInputJobConfig>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import CondDB

CondDB().Upgrade     = True
CondDB().AllLocalTagsByDataType=["VP+UT","FT"]

from Configurables import Gauss

Gauss().DetectorGeo  = { "Detectors": ['VP', 'UT', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['VP', 'UT', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['VP', 'UT', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DataType     = "Upgrade" 
