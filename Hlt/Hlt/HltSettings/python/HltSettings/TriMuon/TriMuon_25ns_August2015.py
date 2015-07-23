# =============================================================================
# @file   TriMuon_25ns_August2015.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   20.07.2015
# =============================================================================
"""Threshold settings for Hlt2 TriMuon lines for 2015."""

from GaudiKernel.SystemOfUnits import GeV, MeV, micrometer


class TriMuon_25ns_August2015(object):
    __all__ = ('ActiveHlt2Lines', 'Thresholds')

    def ActiveHlt2Lines(self):
        """Returns a list of active lines."""

        lines = ['Hlt2TriMuonTau23Mu',
                 'Hlt2TriMuonDetached']

        return lines

    def Thresholds(self):
        """Returns the line thresholds."""
        thresholds = {'Common': {},
                      'GoodMuonsForTriMuonLines': {'TrackChi2': 4,
                                                   'Chi2IP'   : 9},
                      'TriMuonTau23Mu': {'MassWin'   : 225 * MeV,
                                         'DiMuMass'  : 14 * MeV,
                                         'VertexChi2': 25,
                                         'ctau'      : 45 * micrometer},
                      'TriMuonDetached': {'Chi2IP_Tight': 36,
                                          'MuonPT'      : 1.4 * GeV}}
        # Noew build the final dictionary
        from Hlt2Lines.TriMuon.Lines import TriMuonLines

        return {TriMuonLines: thresholds}

# EOF

