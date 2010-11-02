# $Id: StreamCharm.py,v 1.4 2010-09-08 09:05:18 gcowan Exp $
#
#   Charm stripping selection
#
#   all physics channels with hadronic final states from charm decay
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

stream = StrippingStream("CharmControl")

#
# Lines for the RD WG
# Ulrik Egede
#

# Mitesh Patel, Thomas Blake

from StrippingSelections.StrippingDKMuMu import StrippingDKMuMuConf
from StrippingSelections.StrippingDKMuMu import defaultLines as DKMuMuLines
from StrippingSelections.StrippingDKMuMu import defaultConfig as DKMuMuConfig

DKMuMuConf = StrippingDKMuMuConf( config=DKMuMuConfig, activeLines= DKMuMuLines)
stream.appendLines( DKMuMuConf.lines() )

#
# Lines from Semileptonic WG
# Rob Lambert
#

# Marta Calvi, Stefania Vecchi
from StrippingSelections.StrippingDstarVeryLooseWithD02Kpi import DstarVeryLooseWithD02KpiAllLinesConf
from StrippingSelections.StrippingDstarVeryLooseWithD02Kpi import confdict as DstarVeryLooseWithD02KpiConfDict

DstarVeryLooseWithD02Kpi = DstarVeryLooseWithD02KpiAllLinesConf( DstarVeryLooseWithD02KpiConfDict )
stream.appendLines( DstarVeryLooseWithD02Kpi.Lines )

# Liming Zhang
from StrippingSelections.StrippingD0ForBXX import StrippingD0forBXXConf
stream.appendLines( [ StrippingD0forBXXConf().D02KPiforBXXLine(),
                      StrippingD0forBXXConf().D02K3PiforBXXLine() ] )

from StrippingSelections.StrippingDForBSemi import StrippingDforBSemiConf
stream.appendLines( [ StrippingDforBSemiConf().DpforBSemiLine(),
                      StrippingDforBSemiConf().DsforBSemiLine(),
                      StrippingDforBSemiConf().LcforBSemiLine() ] )

#
# Lines for the Flavour WG
# Jibo He, Olivier Schneider
#

from StrippingSelections.StrippingCcbar2Ppbar import StrippingCcbar2PpbarConf
stream.appendLines( [  StrippingCcbar2PpbarConf().Nominal_Line(),
                       StrippingCcbar2PpbarConf().Exclusive_Line() ])


#
# Lines from Gamma from trees
# Tim Gershon, Anton Poluektov, Andrea Contu
#

from StrippingSelections.StrippingDstar_D2KPiPi0 import lineWS, lineRS
stream.appendLines( [ lineWS, lineRS ] )


