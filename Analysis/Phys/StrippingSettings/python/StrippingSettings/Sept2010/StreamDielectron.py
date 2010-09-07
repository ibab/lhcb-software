# $Id: StreamDielectron.py,v 1.1 2010-09-07 11:57:00 gcowan Exp $
#
#   Dielectron stripping selection
#
#   all physics channels with dielectron in the final state
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Dielectron")

#
# Lines from Flavour WG
# Jibo He
#
from StrippingSelections.StrippingDiElectron import StrippingDiElectronConf
stream.appendLines( [ StrippingDiElectronConf().Jpsi2ee_line() ] )
# Following lines (not the low mass ones) are taking ~0.2%. This is too high
#stream.appendLines( [ StrippingDiElectronConf().IncDiElectron_line() ] )
#stream.appendLines( [ StrippingDiElectronConf().IncDiElectronLowMass_line() ] )
#stream.appendLines( [ StrippingDiElectronConf().BiasedIncDiElectron_line() ] )
#stream.appendLines( [ StrippingDiElectronConf().BiasedIncDiElectronLowMass_line() ] )

#
# Lines from beta_s WG
# Jibo He
#
from StrippingSelections.StrippingBs2JpsieePhi import StrippingBs2JpsieePhiConf
stream.appendLines( [ StrippingBs2JpsieePhiConf().UnbiasedPT_line() ] )
stream.appendLines( [ StrippingBs2JpsieePhiConf().Signalbox_line() ] )

#
# Lines from RD WG
# Ulrik Egede, Jibo He
#
from StrippingSelections.StrippingBd2eeKstar import Bd2eeKstarConf
stream.appendLines( [ Bd2eeKstarConf().line ] )

from StrippingSelections.StrippingBd2JpsieeKstar import Bd2JpsieeKstarConf
stream.appendLines( [ Bd2JpsieeKstarConf().line ])
