from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPMuonLines() :
    def localcuts(self) :
        return {    'DiMuon'    :   {'nVeloTracksmax'   :     10,
                                     'nBackTracksmax'   :     1,
                                     'AMmin'            :     0.0 * GeV,
                                     'mu_PTmin'         :     400 * MeV},
                    'DiMuon_PS' :   {'nVeloTracksmax'   :     10,
                                     'nBackTracksmax'   :     1,
                                     'AMmin'            :     0.0 * GeV,
                                     'mu_PTmin'         :     400 * MeV},
                    'Muon'      :   {'nVeloTracksmax'   :     4,
                                     'nBackTracksmax'   :     1,
                                     'mu_PTmin'         :     400 * MeV},
               }

    def locallines(self):
      from Stages import LowMultDiMuonFilter, LowMultMuonFilter
      stages = { 'LowMultDiMuon'      : [LowMultDiMuonFilter('DiMuon')],
                 'LowMultDiMuon_PS'   : [LowMultDiMuonFilter('DiMuon_PS')],
                 'LowMultMuon'        : [LowMultMuonFilter('Muon')],
               }
      return stages
