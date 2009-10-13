#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingRICHLambdaNoPID
from StrippingSelections import StrippingMuIDCalib

stream = StrippingStream("Lambda")
stream.appendLines( [ 
		        StrippingRICHLambdaNoPID.line, 
		        StrippingMuIDCalib.line_lambda_dd, 
		        StrippingMuIDCalib.line_lambda_ll 
		    ] )
