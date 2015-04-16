from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CEPTechnicalLines() :
    def localcuts(self) :
        return {    'NonBeamBeamMinActivity'    :   {'nVeloTracksmax'   :     10,
                                     'nBackTracksmax'   :     1,
                                     'AMmin'            :     0.0 * GeV,
                                     'mu_PTmin'         :     400 * MeV},
                    'NonBeamBeamNoBias' :   {}
               }

    def locallines(self):
      from Stages import LowMultDiMuonFilter, LowMultMuonFilter
      stages = { 'LowMultNonBeamBeamMinActivity'      : [LowMultNonBeamBeamMinActivityFilter('NonBeamBeamMinActivity')],
                 'LowMultNonBeamBeamNoBias'           : [LowMultNonBeamBeamNoBiasFilter('NonBeamBeamNoBias')]
               }
      return stages
