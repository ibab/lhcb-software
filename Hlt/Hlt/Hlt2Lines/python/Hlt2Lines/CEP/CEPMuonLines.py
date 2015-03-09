from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPMuonLines(Hlt2LinesConfigurableUser) :
    def localcuts(self) :
        return {    'DiMuon'    :   {'nVeloTracksmax'   :     4,
                                     'mu_PTmin'         :     400 * MeV},
                    'Muon'      :   {'nVeloTracksmax'   :     4,
                                     'mu_PTmin'         :     400 * MeV},
               }

    def locallines(self):
      from Stages import LowMultDiMuonFilter, LowMultMuonFilter
      stages = { 'LowMultDiMuon'      : [LowMultDiMuonFilter('DiMuon')],
                 'LowMultMuon'        : [LowMultMuonFilter('Muon')],
               }
      return stages
