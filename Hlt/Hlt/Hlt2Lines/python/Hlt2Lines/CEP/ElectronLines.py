from GaudiKernel.SystemOfUnits import MeV
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPElectronLines() :
    def localcuts(self) :
        return {'Electron'      :   {'nVeloTracksmax'  :     8,
                                     'e_PTmin'         :     250 * MeV},
                'Electron_noTrFilt':{'e_PTmin'        :     250 * MeV}
               }

    def locallines(self):
      from Stages import LowMultElectronFilter, LowMultElectron_noTrFiltFilter
      stages = {'LowMultElectron'        : [LowMultElectronFilter('Electron')],
                'LowMultElectron_noTrFilt': [LowMultElectron_noTrFiltFilter('Electron')]}
      return stages
