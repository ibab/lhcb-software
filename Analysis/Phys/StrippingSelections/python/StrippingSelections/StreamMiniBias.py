#
# Stripping selections 
# for Minimum Bias physics.
# author: Yasmine Amhis
# date : 2009-11-11
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingMiniBias 
from StrippingSelections import StrippingBeamGas

from StrippingSelections.StrippingPass import StrippingPassConf

stream = StrippingStream("MiniBias")

stream.appendLines( [ StrippingMiniBias.MBMicroBiasLine ,
                      StrippingMiniBias.MBNoBiasLine ,
                      StrippingMiniBias.MBMiniBiasLine ,
                      StrippingBeamGas.lineUnbiased ,
# task?               StrippingPassConf().passLine(),
#                     StrippingPassConf().physLine()
                      ] )
