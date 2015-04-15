from GaudiKernel.SystemOfUnits import MeV
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPElectronLines() :
    def localcuts(self) :
        return {'Electron'      :   {'nVeloTracksmax'  :     8,
                                     'nBackTracksmax'  :     1,
                                     'e_PTmin'         :     250 * MeV},
                'Electron_noTrFilt':{'nVeloTracksmax'  :     10000,
                                     'nBackTracksmax'  :     10000,
                                     'e_PTmin'         :     250 * MeV}
               }

    def locallines(self):
      from Stages import LowMultElectronFilter
      stages = {'LowMultElectron'         : [LowMultElectronFilter('Electron')],
                'LowMultElectron_noTrFilt': [LowMultElectronFilter('Electron_noTrFilt')]}
      return stages
