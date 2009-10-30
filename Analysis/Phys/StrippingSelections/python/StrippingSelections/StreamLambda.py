#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingLambdaNoPID
from StrippingSelections import StrippingMuIDCalib

stream = StrippingStream("Lambda")
stream.appendLines( [ 
		        StrippingLambdaNoPID.line_lambda_dd, 
		        StrippingLambdaNoPID.line_lambda_ll 
		    ] )
