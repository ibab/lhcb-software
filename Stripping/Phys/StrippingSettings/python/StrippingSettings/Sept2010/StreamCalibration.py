# $Id: StreamCalibration.py,v 1.3 2010-09-07 16:08:59 gcowan Exp $
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
from StrippingSelections.StrippingV0ForPID import StrippingV0ForPIDConf
stream.appendLines( StrippingV0ForPIDConf().All_Lines() )

# Juan Palacios
from StrippingSelections import StrippingHltErrorBits
stream.appendLines( StrippingHltErrorBits.lines )

# Sebastian Schleich
from StrippingSelections.StrippingInclPhi import InclPhiConf
InclPhiConf = InclPhiConf("InclPhi", InclPhiConf.config_Sept2010 )
stream.appendLines( InclPhiConf.lines )

# Gaia Lanfranchi
from StrippingSelections import StrippingMuIDCalib
stream.appendLines( [ StrippingMuIDCalib.jp_line, StrippingMuIDCalib.b_line ] )

# Philip Xing D*->D(Kpi)pi
from StrippingSelections.StrippingNoPIDDstarWithD02RSKPi import StrippingNoPIDDstarWithD02RSKPiConf
stream.appendLines( StrippingNoPIDDstarWithD02RSKPiConf().lines() )

# Regis Lefevre D0 ->Kpipi0 ??
from StrippingSelections.StrippingD02KPiPi0 import StrippingD02KPiPi0Conf
stream.appendLines( [ StrippingD02KPiPi0Conf().D02KPiPi0_Merged(), StrippingD02KPiPi0Conf().D02KPiPi0_Resolved() ] )

