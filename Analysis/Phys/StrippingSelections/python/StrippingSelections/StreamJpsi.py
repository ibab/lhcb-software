#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingJpsiInc
from StrippingSelections import StrippingMuIDCalib

stream = StrippingStream("Jpsi")
stream.appendLines( [ 
		        StrippingJpsiInc.line, 
		        StrippingMuIDCalib.jp_line, 
		        StrippingMuIDCalib.b_line 
		    ] )
