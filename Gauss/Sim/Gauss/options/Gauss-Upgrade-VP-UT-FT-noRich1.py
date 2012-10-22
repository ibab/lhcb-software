## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Upgrade Detectors: VP UT FT Rich2Pmt
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade-VP-UT-FT-noRich1.py <someInputJobConfig>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import LHCbApp
from Configurables import DDDBConf
from Configurables import CondDB

CondDB().Upgrade     = True
CondDB().AllLocalTagsByDataType=["VP+UT","FT"]
LHCbApp().DDDBtag    = "dddb-20121018"
LHCbApp().CondDBtag  = "simcond-20121001-vc-md100"
LHCbApp().Simulation = True

from Gauss.Configuration import *

Gauss().DetectorGeo  = { "Detectors": ['VP', 'UT', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['VP', 'UT', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['VP', 'UT', 'FT', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DataType     = "Upgrade" 
