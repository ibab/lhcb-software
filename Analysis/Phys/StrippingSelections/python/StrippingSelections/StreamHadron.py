#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingB2Charged2Body
from StrippingSelections import StrippingB2DPi
from StrippingSelections import StrippingBs2PhiPhi
from StrippingSelections import StrippingBu2D0K_D02KSPiPi
from StrippingSelections import StrippingBu2hhh
from StrippingSelections import StrippingTopo
from StrippingSelections import StrippingBs2Kst0Kst0

stream = StrippingStream("Hadron")
stream.appendLines( [ 
		        StrippingB2Charged2Body.line,  
		        StrippingB2DPi.line1,  
		        StrippingB2DPi.line2,  
			StrippingBs2PhiPhi.line1, 
			StrippingBs2PhiPhi.line2, 
		        StrippingBu2D0K_D02KSPiPi.line1,  
		        StrippingBu2D0K_D02KSPiPi.line2,  
		        StrippingBu2hhh.line,  
		        StrippingTopo.line1,
                        StrippingTopo.line2, 
                        StrippingBs2Kst0Kst0.line
		    ] )
