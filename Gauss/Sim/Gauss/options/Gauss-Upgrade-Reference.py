## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Upgrade Detectors: Rich1Pmt Rich2Pmt
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade-Reference.py <someInputJobConfiguration>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import CondDB
CondDB().Upgrade     = True

from Configurables import Gauss

Gauss().DetectorGeo  = { "Detectors": ['Velo', 'PuVeto', 'TT', 'IT', 'OT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['Velo', 'PuVeto', 'TT', 'IT', 'OT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['Velo', 'PuVeto', 'TT', 'IT', 'OT', 'Rich1Pmt', 'Rich2Pmt', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DataType     = "Upgrade" 

