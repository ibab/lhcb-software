## Options to switch off all geometry (and related simulation)
## save that of calorimeters area.
##
## Author: P.Szczypka
## Date:   2012-12-12
##


from Gaudi.Configuration import *

from Configurables import Gauss
Gauss().DetectorGeo  = { "Detectors": ['Spd', 'Prs', 'Ecal', 'Hcal', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['Spd', 'Prs', 'Ecal', 'Hcal', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['Spd', 'Prs', 'Ecal', 'Hcal', 'Magnet' ] }
Gauss().BeamPipe     = "BeamPipeInDet"

# Remove RICH cuts
def switchOffRICHCuts():
    from Configurables import SimulationSvc
    SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml"

appendPostConfigAction( switchOffRICHCuts )
