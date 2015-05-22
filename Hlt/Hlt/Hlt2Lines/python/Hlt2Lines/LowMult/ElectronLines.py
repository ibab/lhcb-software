from GaudiKernel.SystemOfUnits import MeV
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPElectronLines() :
    def localcuts(self) :
        return {'DiElectron'      :   {'nVeloTracksmax'  :     8,
                                       'nBackTracksmax'  :     1,
                                       'e_PTmin'         :     250 * MeV},
                'DiElectron_noTrFilt':{'nVeloTracksmax'  :     10000,
                                       'nBackTracksmax'  :     10000,
                                       'e_PTmin'         :     250 * MeV}
               }

    def locallines(self):
      from Stages import LowMultDiElectronFilter
      stages = {'LowMultDiElectron'         : [LowMultDiElectronFilter('DiElectron')],
                'LowMultDiElectron_noTrFilt': [LowMultDiElectronFilter('DiElectron_noTrFilt')]}
      return stages
