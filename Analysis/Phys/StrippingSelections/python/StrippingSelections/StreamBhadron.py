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

from StrippingSelections import StrippingB2Charged2Body
#from StrippingSelections.StrippingB02D0Kstar_D02KPiPi0 import B02D0Kstar_D02KPiPi0Conf
#from StrippingSelections.StrippingB02D0Kstar_D02K3Pi import B02D0Kstar_D02K3PiConf
from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf #from StrippingSelections import StrippingBs2PhiPhi
from StrippingSelections.StrippingBu2D0K_D02KSPiPi import StrippingBu2D0K_D02KSPiPiConf
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID import StrippingBu2D0h_D02KShh_NoPIDConf
from StrippingSelections import StrippingBu2hhh
from StrippingSelections.StrippingB2KShh import StrippingB2KShhConf
from StrippingSelections import StrippingTopo
#from StrippingSelections import StrippingBs2Kst0Kst0
from StrippingSelections.StrippingBs2Kst0Kst0 import StrippingBs2KstKstConf
from StrippingSelections.StrippingBs2PhiKst0 import StrippingBs2PhiKstConf
from StrippingSelections.StrippingB2hhLTUnbiased import StrippingB2hhLTUnbiasedConf
from StrippingSelections.StrippingB2DX import B2DXLines

from StrippingSelections.StrippingB2D3H import StrippingB2D3HConf, StrippingB2D3HNominal, StrippingB2D3HLoose


stream = StrippingStream("Bhadron")
stream.appendLines( [ 
		        StrippingB2Charged2Body.line,  
			StrippingBs2PhiPhiConf().normal(),  #StrippingBs2PhiPhi.line1, 
			StrippingBs2PhiPhiConf().wide(),    #StrippingBs2PhiPhi.line2, 
		        StrippingBu2D0K_D02KSPiPiConf().Bu2D0K_KSLL(), #StrippingBu2D0K_D02KSPiPi.line1,  
		        StrippingBu2D0K_D02KSPiPiConf().Bu2D0K_KSDD(), #StrippingBu2D0K_D02KSPiPi.line2,
                        StrippingBu2D0h_D02KShh_NoPIDConf().Bu2D0h_KSPiPi_KSLL(),  
		        StrippingBu2D0h_D02KShh_NoPIDConf().Bu2D0h_KSPiPi_KSDD(),
                        StrippingB2KShhConf().B2KSLLhh(),
                        StrippingB2KShhConf().B2KSDDhh(),
     		        StrippingBu2hhh.line,   
			StrippingTopo.line1,
                        StrippingTopo.line2, 
                        StrippingBs2KstKstConf().nominall(),
                        StrippingBs2KstKstConf().loosel(),
                        StrippingBs2PhiKstConf().nominall(),
                        StrippingBs2PhiKstConf().loosel(),
                        StrippingB2hhLTUnbiasedConf().StripB2hhLTUnbiased(),
                        StrippingB2hhLTUnbiasedConf().StripB2hhLTUnbiasedLoose()
		    ] )

stream.appendLines( B2DXLines().lines ) # B2DX lines with default cuts
#stream.appendLines( B02D0Kstar_D02KPiPi0Conf().lines ) 
#stream.appendLines( B02D0Kstar_D02K3PiConf().lines ) 

stream.appendLines( StrippingB2D3HLoose('Loose').lines ) # B2D3H Lines
