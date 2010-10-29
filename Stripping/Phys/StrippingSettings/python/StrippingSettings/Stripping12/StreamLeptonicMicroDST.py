# $Id: StreamCharm.py,v 1.4 2010-09-08 09:05:18 gcowan Exp $
#
#   Dimuon + Dielectron stripping selections for microDST
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
MicroDSTDiElectronConf = DiElectronConf( name = 'MicroDST', config = DiElectronConf.config_microDST )
stream.appendLines( MicroDSTDiElectronConf.MicroDSTLines )

#
# Inclusive Dimuon Lines
# Patrick Robbe
#

from StrippingSelections.StrippingDiMuonNew import DiMuonConf
MicroDSTDiMuonConf = DiMuonConf( name = 'MicroDST', config = DiMuonConf.config_microDST )
stream.appendLines( MicroDSTDiMuonConf.MicroDSTLines )
