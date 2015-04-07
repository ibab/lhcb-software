from GaudiKernel.SystemOfUnits import MeV
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPElectronLines() :
    def localcuts(self) :
        return {'Electron'      :   {'nVeloTracksmax'  :     8,
                                     'e_PTmin'         :     250 * MeV},
                'MinKinBiasElectron':{'e_PTmin'        :     250 * MeV}
               }

    def locallines(self):
      from Stages import LowMultElectronFilter
      stages = {'LowMultElectron'        : [LowMultElectronFilter('Electron')],
                'LowMultMinKinBiasElectron': [LowMultMinKinBiasElectronFilter('Electron')]}
      return stages
