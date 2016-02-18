# =============================================================================
# @file   Majorana_25ns_Draft2016.py
# @author Carla Marin (carla.marin@cern.ch)
# @date   18.02.2016
# =============================================================================
"""Threshold settings for Hlt2 Majorana lines for 2016.
"""


class Majorana_25ns_Draft2016(object):
    """Threshold settings for Hlt2 Majorana lines for 2016."""
    __all__ = ('ActiveHlt2Lines', 'Thresholds')

    def ActiveHlt2Lines(self):
        """Returns a list of active lines."""

        lines = ['Hlt2MajoranaBLambdaMuLL',
                 'Hlt2MajoranaBLambdaMuDD']

        return lines

    def Thresholds(self):
        """Set thresholds for the lines."""

        from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
        thresholds = {'Prescale' : {},
                      'Common' : {'TrChi2' : 3,
                                  'TrGP' : 0.3},
                      'LambdaMuPi': {"MuonGHOSTPROB"         : 0.5   ,#adimensional
                                     "MuonTRCHI2"            : 4.    ,#adimensional
                                     "MuonP"                 : 3000.* MeV,
                                     "MuonPT"                : 250. * MeV,
                                     "MuonPIDK"              : 0.    ,#adimensional
                                     "MuonPIDmu"             : 0.    ,#adimensional
                                     "MuonPIDp"              : 0.    ,#adimensional
                                     "MuonMINIPCHI2"         : 12    ,#adminensional
                                     #Lambda Daughter Cuts
                                     "Lambda0DaugP"          : 2000.* MeV,
                                     "Lambda0DaugPT"         : 250. * MeV,
                                     "Lambda0DaugTrackChi2"  : 4.    ,#adimensional
                                     "Lambda0DaugMIPChi2"    : 10.   ,#adimensional
                                     #Lambda cuts
                                     "MajoranaCutFDChi2"     : 100.,  #adimensional
                                     "MajoranaCutM"          : 1500. * MeV,
                                     "Lambda0VertexChi2"     : 10.   ,#adimensional
                                     "Lambda0PT"             : 700.  ,#adimensional
                                     },
                      'BLambdaMu':  {#B Mother Cuts
                                     "BVCHI2DOF"             : 4.    ,#adminensional
                                     "BDIRA"                 : 0.99  ,#adminensional
                                     "LambdaMuMassLowTight"  : 1500.  * MeV,
                                     "XMuMassUpperHigh"      : 6500. * MeV,
                                     'LambdaZ'               : 0. * mm
                                     }
                      }

        from Hlt2Lines.Majorana.Lines import MajoranaLines
        
        return {MajoranaLines: thresholds}

# EOF
