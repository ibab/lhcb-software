# =============================================================================
# @file   RareStrange_25ns_August2015.py
# @author Albert Puig (albert.puig@cern.ch)
# @date   20.07.2015
# =============================================================================
"""Threshold settings for Hlt2 Rare Strange lines for 2015."""

from GaudiKernel.SystemOfUnits import mm, MeV, picosecond


class RareStrange_25ns_August2015(object):
    __all__ = ('ActiveHlt2Lines', 'Thresholds')

    def ActiveHlt2Lines(self):
        """Returns a list of active lines."""

        lines = ['Hlt2RareStrangeSigmaPMuMu',
                 'Hlt2RareStrangeKPiMuMu']

        return lines

    def Thresholds(self):
        """Returns the line thresholds."""
        thresholds = {'Common': {'TrChi2': 3,
                                 'TrGP'  : 0.3},
                      'SigmaPMuMu': {'muonMinIpChi2' : 25.,
                                     'pPIDp'         : 5.,
                                     'pMinIpChi2'    : 25.,
                                     'SigmaMassWin'  : 500 * MeV,
                                     'SigmaMaxDOCA'  : 2. * mm,
                                     'SigmaVtxChi2'  : 25,   # adimensional
                                     'SigmaMinPt'    : 500 * MeV,
                                     'SigmaMinDIRA'  : 0.9,  # adimensional
                                     'SigmaMaxIpChi2': 36,   # adimensional
                                     'SigmaMinTauPs' : 6 * picosecond},
                      'KPiMuMu': {'muonMinIpChi2':  25.,
                                  'piMinIpChi2'  :  25.,
                                  'KMassWin'     : 500 * MeV,
                                  'KMaxDOCA'     : 2. * mm,
                                  'KVtxChi2'     : 25.,  # adimensional
                                  'KMinPt'       : 500 * MeV,
                                  'KMinDIRA'     : 0.9,  # adimensional
                                  'KMaxIpChi2'   : 36,   # adimensional
                                  'KMinTauPs'    : 10 * picosecond}}
        # Noew build the final dictionary
        from Hlt2Lines.RareStrange.Lines import RareStrangeLines

        return {RareStrangeLines: thresholds}

# EOF

