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

# AcceptBadEvents means that events which fail the
# reconstruction are still written out.

stream = StrippingStream("Calibration", AcceptBadEvents = True)

# Juan Palacios
from StrippingSelections import StrippingHltErrorBits
stream.appendLines( StrippingHltErrorBits.lines )

#
# Lines for RICH PID
# Andrew Powell 
#

# Andrew Powell
from StrippingSelections.StrippingV0ForPID import StrippingV0ForPIDConf
stream.appendLines( StrippingV0ForPIDConf().All_Lines() )

# Philip Xing D*->D(Kpi)pi
from StrippingSelections.StrippingNoPIDDstarWithD02RSKPi import StrippingNoPIDDstarWithD02RSKPiConf
stream.appendLines( StrippingNoPIDDstarWithD02RSKPiConf().Lines() )

# Sebastian Schleich
from StrippingSelections.StrippingInclPhi import InclPhiConf
InclPhi =  InclPhiConf("InclPhi", InclPhiConf.config_default )
stream.appendLines( [ InclPhi.InclPhiHighPtLine,
                      InclPhi.InclPhiLowPtLine ] )

#
# Lines for Photon / Electron ID
# Jibo He, Olivier Deschamps
#

# Regis Lefevre D0 ->Kpipi0
from StrippingSelections.StrippingD02KPiPi0 import StrippingD02KPiPi0Conf
stream.appendLines( [ StrippingD02KPiPi0Conf().D02KPiPi0_Merged(),
                      StrippingD02KPiPi0Conf().D02KPiPi0_Resolved() ] )


# Jibo He
from StrippingSelections.StrippingElectronID import ElectronIDConf
StrippingElectronIDConf =  ElectronIDConf( name = 'Jpsi2eeForElectronID', config = ElectronIDConf.config_default )
stream.appendLines( [ StrippingElectronIDConf.line ])

#
# Lines for Muon ID
# Gaia Lanfranchi, Erica Polycarpo
#

from StrippingSelections import StrippingMuIDCalib
stream.appendLines( [ StrippingMuIDCalib.jp_line,
                      StrippingMuIDCalib.b_line,
                      StrippingMuIDCalib.bnomip_line ] )

#
# Lines for tracking studies
# Michel de Cian, Georg Kroecker 
#

from StrippingSelections.StrippingTrackEffMuonTT import StrippingTrackEffMuonTTConf
from StrippingSettings.Stripping12.LineConfigDictionaries import TrackEffMuonTTConfig
MuonTTStripping = StrippingTrackEffMuonTTConf('TrackEffMuonTT', TrackEffMuonTTConfig )
stream.appendLines( MuonTTStripping.lines )

from StrippingSelections.StrippingTrackEffDownMuon import StrippingTrackEffDownMuonConf
stream.appendLines( [ StrippingTrackEffDownMuonConf().nominal_line() ] )

from StrippingSelections.StrippingTrackEffVeloMuon import StrippingTrackEffVeloMuonConf
stream.appendLines( [ StrippingTrackEffVeloMuonConf().nominal_line() ] )
