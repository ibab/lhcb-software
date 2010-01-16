#
# Stripping selections 
# for Minimum Bias physics.
# author: Yasmine Amhis
# date : 2009-11-11
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections.StrippingV0 import StrippingV0Conf
from StrippingSelections import StrippingV0Geo

stream = StrippingStream("V0")

stream.appendLines( StrippingV0Conf().K0S() )
stream.appendLines( StrippingV0Conf().Lambda0() )
stream.appendLines( [ StrippingV0Geo.line_KS_all, 
                      StrippingV0Geo.line_lambda_all ])
