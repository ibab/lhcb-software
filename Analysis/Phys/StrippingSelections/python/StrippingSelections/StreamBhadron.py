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
from StrippingSelections import StrippingB2DPi
from StrippingSelections import StrippingB2D3Pi
from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf #from StrippingSelections import StrippingBs2PhiPhi
from StrippingSelections.StrippingBu2D0K_D02KSPiPi import StrippingBu2D0K_D02KSPiPiConf
from StrippingSelections.StrippingBu2D0h_D02KShh_NoPID import StrippingBu2D0h_D02KShh_NoPIDConf
from StrippingSelections import StrippingBu2hhh
from StrippingSelections.StrippingB2KSKK_KSpipi import StrippingB2KSKK_KSpipiConf
from StrippingSelections import StrippingBu2hhh_1e4
from StrippingSelections import StrippingTopo
#from StrippingSelections import StrippingBs2Kst0Kst0
from StrippingSelections.StrippingBs2Kst0Kst0 import StrippingBs2KstKstConf

stream = StrippingStream("Bhadron")
stream.appendLines( [ 
		        StrippingB2Charged2Body.line,  
		        StrippingB2DPi.line1,  
		        StrippingB2DPi.line2,  
                        StrippingB2D3Pi.line1,
                        StrippingB2D3Pi.line2,
			StrippingBs2PhiPhiConf().normal(),  #StrippingBs2PhiPhi.line1, 
			StrippingBs2PhiPhiConf().wide(),    #StrippingBs2PhiPhi.line2, 
		        StrippingBu2D0K_D02KSPiPiConf().Bu2D0K_KSLL(), #StrippingBu2D0K_D02KSPiPi.line1,  
		        StrippingBu2D0K_D02KSPiPiConf().Bu2D0K_KSDD(), #StrippingBu2D0K_D02KSPiPi.line2,
                        StrippingBu2D0h_D02KShh_NoPIDConf().Bu2D0h_KSPiPi_KSLL(),  
		        StrippingBu2D0h_D02KShh_NoPIDConf().Bu2D0h_KSPiPi_KSDD(),
			StrippingB2KSKK_KSpipiConf().B2KSLLKK(),
			StrippingB2KSKK_KSpipiConf().B2KSDDKK(),
			StrippingB2KSKK_KSpipiConf().B2KSLLpipi(),
			StrippingB2KSKK_KSpipiConf().B2KSDDpipi(),
     		        StrippingBu2hhh.line,  
		        StrippingBu2hhh_1e4.line,  
			StrippingTopo.line1,
                        StrippingTopo.line2, 
                        StrippingBs2KstKstConf().nominall(),
                        StrippingBs2KstKstConf().loosel()
		    ] )
