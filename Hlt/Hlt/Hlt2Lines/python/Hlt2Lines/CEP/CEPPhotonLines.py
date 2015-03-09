from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPPhotonLines(Hlt2LinesConfigurableUser) :
    def localcuts(self) :
        return { 'Photon'    :   {'gamma_PTmin'      :     250 * MeV},
               }

    def locallines(self):
      from Stages import LowMultPhotonFilter
      stages = { 'LowMultPhoton'      : [LowMultPhotonFilter('Photon')],
               }
      return stages
