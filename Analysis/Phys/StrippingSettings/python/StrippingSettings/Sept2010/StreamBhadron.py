# $Id: StreamBhadron.py,v 1.2 2010-09-07 13:49:53 gcowan Exp $
#
#   Bhadron stripping selection
#
#   all physics channels with hadronic final states from b decay
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("Bhadron")

#
# Lines from Flavour WG
# Rolf Olderman
#
from StrippingSelections.StrippingB2twobody import B2twobodyLine
stream.appendLines( B2twobodyLine() )

#
# Lines from Gamma from loops WG
# Vincenzo Vagnoni and Ulrich Kerzel
#
from StrippingSelections.StrippingB2Charged2Body import line as StrippingB2Charged2BodyLine
stream.appendLines ( [ StrippingB2Charged2BodyLine ] )

from StrippingSelections.StrippingBu2hhh import line as StrippingBu2hhhLine 
stream.appendLines ( [ StrippingBu2hhhLine ] )

from StrippingSelections.StrippingB2KShh import StrippingB2KShhConf
stream.appendLines ( [ StrippingB2KShhConf().B2KSLLhh(), StrippingB2KShhConf().B2KSDDhh() ] )

from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf
stream.appendLines ( [ StrippingB2HHPi0Conf().B2HHPi0_Resolved(), StrippingB2HHPi0Conf().B2HHPi0_Merged() ] )

#
# Lines from Gamma from trees WG
# Tim Gershon, Anton Poluektov, Vava Gligorov
#
from StrippingSelections.StrippingB2DX import B2DXLines
stream.appendLines ( B2DXLines().lines )

from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID import _StrippingBu2D0h_D02KShh_NoPIDConf as Bu2D0h_D02KShh_NoPIDConf
stream.appendLines ( [ Bu2D0h_D02KShh_NoPIDConf.Line_LL, Bu2D0h_D02KShh_NoPIDConf.Line_DD ] )

# Steve Blusk - do we need the Signal lines?
#from StrippingSelections.StrippingB2D3HX import StrippingB2D3HConf
#stream.appendLines ( StrippingB2D3HConf().lines )

#
# Lines from beta_s WG
# Olivier Leroy, Wouter Hulsebergen and Greig Cowan
#
from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
Bs2PhiPhiConf = StrippingBs2PhiPhiConf("Bs2PhiPhi", StrippingBs2PhiPhiConf.config_Sept2010 )
stream.appendLines( Bs2PhiPhiConf.lines )


