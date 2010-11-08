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
# Ulrik Egede, Walter Bonivento
#

# Mitesh Patel, Thomas Blake
from StrippingSelections.StrippingDKMuMu import StrippingDKMuMuConf
from StrippingSelections.StrippingDKMuMu import defaultLines as DKMuMuLines
from StrippingSelections.StrippingDKMuMu import defaultConfig as DKMuMuConfig

DKMuMuConf = StrippingDKMuMuConf( config=DKMuMuConfig, activeLines= DKMuMuLines)
stream.appendLines( DKMuMuConf.lines() )

# Walter Bonivento, D0 -> mu mu searches
from StrippingSelections.StrippingD02MuMu import lines as D02MuMuLines
stream.appendLines( D02MuMuLines )

from StrippingSelections.StrippingJpsippForD0MuMu import StrippingJpsippForD0MuMuConf
stream.appendLines( [ StrippingJpsippForD0MuMuConf().ForD0mumu_Line() ] )

# Benoit Viaud ( 4-body SCS lines )
from StrippingSelections.StrippingDstarD02KKpipiRegular import StrippingDstarD02KKpipiRegularConf
from StrippingSettings.Stripping12.LineConfigDictionaries import D02KKpipiRegularConfig
DstarD0KKpipi = StrippingDstarD02KKpipiRegularConf('DstarD02KKpipiRegular',D02KKpipiRegularConfig)
stream.appendLines( DstarD0KKpipi.lines )

# Benoit Viaud (D*+ -> D0 pi+, D0 -> K- K+ mu- mu+)
from StrippingSelections.StrippingDstarD02KKmumuRegular import StrippingDstarD02KKmumuRegularConf
DstarD0KKmumu = StrippingDstarD02KKmumuRegularConf('DstarD02KKmumuRegular')
stream.appendLines( DstarD0KKmumu.lines )

# Phillip Urquijo
from StrippingSelections.StrippingD2XMuMuSS import StrippingD2XMuMuSSConf
from StrippingConf.StrippingStream import StrippingStream
stream.appendLines( [ StrippingD2XMuMuSSConf().PiSS_line(),
                      StrippingD2XMuMuSSConf().PiOS_line(),
                      StrippingD2XMuMuSSConf().KSS_line(),
                      StrippingD2XMuMuSSConf().KOS_line() ] )

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


