from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPMuonLines() :
    def localcuts(self) :
        return {    'CEPDiMuon'    :   {'nVeloTracksmax'   :     4,
                                     'mu_PTmin'         :     400 * MeV},
                    'CEPMuon'      :   {'nVeloTracksmax'   :     4,
                                     'mu_PTmin'         :     400 * MeV},
               }

    def locallines(self):
      from Stages import LowMultDiMuonFilter, LowMultMuonFilter
      stages = { 'LowMultDiMuon'      : [LowMultDiMuonFilter('CEPDiMuon')],
                 'LowMultMuon'        : [LowMultMuonFilter('CEPMuon')],
               }
      return stages
