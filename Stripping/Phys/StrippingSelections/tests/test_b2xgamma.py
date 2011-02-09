#!/usr/bin/env python

from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingB2XGamma import StrippingB2XGammaConf as builder

config_params = {# Selection parameters
                 'TrIPchi2Phi'          : 10.       # Dimensionless
                 ,'TrIPchi2Kst'         : 10.       # Dimensionless
                 ,'PhiMassWin'          : 15.       # MeV
                 ,'KstMassWin'          : 100.      # MeV
                 ,'KstMassWinSB'        : 200.      # MeV
                 ,'BsMassWin'           : 1000.     # MeV
                 ,'B0MassWin'           : 1000.     # MeV
                 ,'BMassWinSB'          : 2000.     # MeV
                 ,'BsDirAngle'          : 0.02      # radians
                 ,'B0DirAngle'          : 0.02      # radians
                 ,'BDirAngleMoni'       : 0.06      # radians
                 ,'BsPVIPchi2'          : 15.       # Dimensionless
                 ,'B0PVIPchi2'          : 15.       # Dimensionless
                 ,'photonPT'            : 2600.     # MeV
                 ,'PhiVCHI2'            : 15.       # dimensionless
                 ,'KstVCHI2'            : 15.       # dimensionless
                 ,'TrChi2'              : 5.        # dimensionless
                 # Pre- and postscales
                 ,'Bs2PhiGammaWideBMassPreScale'      : 0.1
                 ,'Bs2PhiGammaWideBMassPostScale'     : 1.0
                 ,'Bs2PhiGammaLooseDiraPreScale'      : 0.1
                 ,'Bs2PhiGammaLooseDiraPostScale'     : 1.0
                 ,'Bs2PhiGammaPreScale'               : 1.0
                 ,'Bs2PhiGammaPostScale'              : 1.0
                 ,'Bd2KstGammaWideBMassPreScale'      : 0.05
                 ,'Bd2KstGammaWideBMassPostScale'     : 1.0
                 ,'Bd2KstGammaLooseDiraPreScale'      : 0.05
                 ,'Bd2KstGammaLooseDiraPostScale'     : 1.0
                 ,'Bd2KstGammaWideKstMassPreScale'    : 0.05
                 ,'Bd2KstGammaWideKstMassPostScale'   : 1.0
                 ,'Bd2KstGammaWidePreScale'           : 0.05
                 ,'Bd2KstGammaWidePostScale'          : 1.0
                 ,'Bd2KstGammaPreScale'               : 1.0
                 ,'Bd2KstGammaPostScale'              : 1.0
                  }


from StrippingUtils import LineBuilderTests

def test_line_builder() :
    LineBuilderTests.test_line_builder(builder, config_params)

if __name__ == '__main__' :
    test_line_builder()
