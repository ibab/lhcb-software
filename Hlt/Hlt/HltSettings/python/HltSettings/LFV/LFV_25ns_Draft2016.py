# =============================================================================
# @file   LFV_25ns_Draft2016.py
# @author Carla Marin (carla.marin@cern.ch)
# @date   12.02.2016
# =============================================================================
"""Threshold settings for Hlt2 LFV lines for 2016.
"""


class LFV_25ns_Draft2016(object):
    """Threshold settings for Hlt2 LFV lines for 2016."""
    __all__ = ('ActiveHlt2Lines', 'Thresholds')

    def ActiveHlt2Lines(self):
        """Returns a list of active lines."""

        lines = ['Hlt2LFVJpsiMuETurbo']

        return lines

    def Thresholds(self):
        """Set thresholds for the lines."""

        from GaudiKernel.SystemOfUnits import MeV
        m_jpsi = 3096
        thresholds = {'Prescale': {},
                      'Common': {},
                      'JpsiMuE': {'CombMassHigh': (m_jpsi + 550) * MeV,
                                  'CombMassLow': (m_jpsi - 1100) * MeV,
                                  'ElectronProbNn': 0.93,
                                  'ElectronTrChi2DoF': 3,
                                  'ElectronTrGhostProb': 0.2,
                                  'MassHigh': (m_jpsi + 500) * MeV,
                                  'MassLow': (m_jpsi - 1000) * MeV,
                                  'MuonProbNn': 0.9,
                                  'MuonTrChi2DoF': 3,
                                  'MuonTrGhostProb': 0.1,
                                  'VertexChi2DoF': 3},
                      'SpdCut': {'NSPD': 250}}

        from Hlt2Lines.LFV.Lines import LFVLines
        
        return {LFVLines: thresholds}

# EOF
