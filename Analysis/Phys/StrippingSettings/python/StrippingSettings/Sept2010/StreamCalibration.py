# $Id: StreamCalibration.py,v 1.1 2010-09-07 11:57:00 gcowan Exp $
#
#   Calibration stripping selections
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

# AcceptBadEvents means that events which fail the reconstruction are still written out
stream = StrippingStream("Calibration", AcceptBadEvents = True)

# Andrew Powell
from StrippingSelections import StrippingLambdaNoPID
stream.appendLines( [ StrippingLambdaNoPID.line_lambda_dd, StrippingLambdaNoPID.line_lambda_ll ] )

# KS -> pipi ??

# Juan Palacios
from StrippingSelections import StrippingHltErrorBits
stream.appendLines( StrippingHltErrorBits.lines )

# Sebastian Schleich
from StrippingSelections.StrippingInclPhi import InclPhiConf
InclPhiConf = InclPhiConf("InclPhi", InclPhiConf.config_Sept2010 )
stream.appendLines( MyInclPhiConf.lines )

# Gaia Lanfranchi
from StrippingSelections import StrippingMuIDCalib
stream.appendLines( [ StrippingMuIDCalib.jp_line, StrippingMuIDCalib.b_line ] )

# D*->D(Kpi)pi ??

# D0 ->Kpipi0 ??


