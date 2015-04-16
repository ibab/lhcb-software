from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPPhotonLines() :
    def localcuts(self) :
        return { 'DiPhoton'           :   {'nVeloTracksmax'   :     10
                                          ,'nBackTracksmax'   :     1
                                          ,'gamma_PTmin'      :     200 * MeV
                                          ,'digamma_AMmin'    :     0.0 * GeV
                                          ,'digamma_AMmax'    :     5.0 * GeV},
                 'DiPhoton_HighMass'  :   {'nVeloTracksmax'   :     10
                                          ,'nBackTracksmax'   :     1
                                          ,'gamma_PTmin'      :     200 * MeV
                                          ,'digamma_AMmin'    :     2.0 * GeV
                                          ,'digamma_AMmax'    :     5.0 * GeV},
                 'pi0'                :   {'nVeloTracksmax'   :     10
                                          ,'nBackTracksmax'   :     1
                                          ,'pi0_PTmin'        :     200 * MeV}
               }

    def locallines(self):
      from Stages import LowMultDiPhotonFilter, LowMultPi0Filter
      stages = { 'LowMultDiPhoton'            : [LowMultDiPhotonFilter('DiPhoton')],
                 'LowMultDiPhoton_HighMass'   : [LowMultDiPhotonFilter('DiPhoton_HighMass')],
                 'LowMultPi0'                 : [LowMultPi0Filter('pi0')]
               }
      return stages
