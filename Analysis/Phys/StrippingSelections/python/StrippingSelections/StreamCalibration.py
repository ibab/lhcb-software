#
#   Calibration stripping selections 
#
#   Calibration channels: Lambda->Kpi, J/psi->mumu and 
#   J/psi->ee selected without PID requirements
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingLambdaNoPID
from StrippingSelections import StrippingMuIDCalib
from StrippingSelections import StrippingHltErrorBits
from StrippingSelections.StrippingInclPhi import InclPhiConf



stream = StrippingStream("Calibration")
stream.appendLines( [ 
		        StrippingMuIDCalib.jp_line, 
		        StrippingMuIDCalib.b_line, 
		        StrippingLambdaNoPID.line_lambda_dd, 
		        StrippingLambdaNoPID.line_lambda_ll
		    ] )

MyInclPhiConf =  InclPhiConf("InclPhi", InclPhiConf.config_default )
stream.appendLines( MyInclPhiConf.lines )


stream.appendLines (StrippingHltErrorBits.lines)
