#
#   Bhadron stripping selections 
#
#  all physics channels with hadronic final states from a B decay
#    - B exclusive selections like: B->D(KSpipi),B->phiphi,B->K*K* etc.
#    - B semi-inclusive selections: B->DX
#    - Topological selections
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream
#from StrippingSelections.StrippingB2twobody_prompt import B2twobody_promptLine
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
#from StrippingSelections.StrippingB02D0Kstar_D02KPiPi0 import B02D0Kstar_D02KPiPi0Conf
#from StrippingSelections.StrippingB02D0Kstar_D02K3Pi import B02D0Kstar_D02K3PiConf
from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID import _StrippingBu2D0h_D02KShh_NoPIDConf
from StrippingSelections import StrippingBu2hhh
from StrippingSelections import StrippingBu3hFrom2h
from StrippingSelections.StrippingB2KShh import StrippingB2KShhConf
from StrippingSelections import StrippingTopo
from StrippingSelections.StrippingBs2Kst0Kst0 import StrippingBs2KstKstConf
from StrippingSelections.StrippingBs2PhiKst0 import StrippingBs2PhiKstConf
from StrippingSelections.StrippingB2hhLTUnbiased import StrippingB2hhLTUnbiasedConf

from StrippingSelections.StrippingB2DX import B2DXLines

from StrippingSelections.StrippingB2D3H import StrippingB2D3HConf, StrippingB2D3HNominal, StrippingB2D3HLoose


stream = StrippingStream("Bhadron")


MyStrippingBs2PhiPhiConf =  StrippingBs2PhiPhiConf("Bs2PhiPhi", StrippingBs2PhiPhiConf.config_default )
stream.appendLines( MyStrippingBs2PhiPhiConf.lines )

stream.appendLines( Hb2Charged2BodyLines().lines() )


stream.appendLines( [ 
    #	       
	                _StrippingBu2D0h_D02KShh_NoPIDConf.Line_LL,  
		        _StrippingBu2D0h_D02KShh_NoPIDConf.Line_DD,
                        StrippingB2KShhConf().B2KSLLhh(),
                        StrippingB2KShhConf().B2KSDDhh(),
     		        StrippingBu2hhh.line,
                        StrippingBu3hFrom2h.line1,
                        StrippingBu3hFrom2h.line2,
			StrippingTopo.line1,
                        StrippingTopo.line2, 
                        StrippingBs2KstKstConf().nominall(),
                        StrippingBs2PhiKstConf().nominall(),
                        StrippingBs2PhiKstConf().loosel(),
#                        StrippingB2hhLTUnbiasedConf().StripB2hhLTUnbiased(),
#                        StrippingB2hhLTUnbiasedConf().StripB2hhLTUnbiasedLoose()
		    ] )

stream.appendLines( B2DXLines().lines ) # B2DX lines with default cuts
#stream.appendLines( B02D0Kstar_D02KPiPi0Conf().lines ) 
#stream.appendLines( B02D0Kstar_D02K3PiConf().lines ) 
#stream.appendLines( B2twobody_promptLine() )
stream.appendLines( StrippingB2D3HLoose('Loose').lines ) # B2D3H Lines
