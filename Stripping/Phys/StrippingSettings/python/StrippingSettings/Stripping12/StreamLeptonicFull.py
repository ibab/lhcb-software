# $Id: StreamCharm.py,v 1.4 2010-09-08 09:05:18 gcowan Exp $
#
#   Dimuon + Dielectron stripping selections for DST
#   This is a copy of the selections on the microDST and
#   should not be prescaled.
#
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Leptonic")

#
# Inclusive Dielectron Lines
# Jibo He
#

from StrippingSelections.StrippingDiElectronNew import DiElectronConf
FullDSTDiElectronConf = DiElectronConf( config = DiElectronConf.config_default )
stream.appendLines( FullDSTDiElectronConf.Lines )

#
# Inclusive Dimuon Lines
# Patrick Robbe
#

from StrippingSelections.StrippingDiMuonNew import DiMuonConf
FullDSTDiMuonConf = DiMuonConf( config = DiMuonConf.config_default )
stream.appendLines( FullDSTDiMuonConf.Lines )
