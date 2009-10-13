#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingBs2JpsiPhi
from StrippingSelections import StrippingBs2JpsiPhiDetached
from StrippingSelections import StrippingBs2JpsiPhiPrescaled
from StrippingSelections import StrippingBd2JpsiKS
from StrippingSelections import StrippingBd2JpsiKSBiased
from StrippingSelections import StrippingBd2JpsiKstar
from StrippingSelections import StrippingBd2JpsiKstarDetached
from StrippingSelections import StrippingBd2JpsiKstarDetachedR
from StrippingSelections import StrippingBd2KstarMuMu
from StrippingSelections import StrippingBs2Jpsif0
from StrippingSelections import StrippingBiasedBs2JpsiPhi
from StrippingSelections import StrippingBu2JpsiK
from StrippingSelections import StrippingBu2JpsiKDetached
from StrippingSelections import StrippingBu2JpsiKDetachedR

stream = StrippingStream("Bmuon")
stream.appendLines( [ 
		        StrippingBs2JpsiPhi.line,  
		        StrippingBs2JpsiPhiPrescaled.line,  
			StrippingBs2JpsiPhiDetached.line, 
		        StrippingBd2JpsiKS.line,  
		        StrippingBd2JpsiKSBiased.line,  
		        StrippingBd2JpsiKstar.line,  
		        StrippingBd2JpsiKstarDetached.line,  
		        StrippingBd2JpsiKstarDetachedR.line,  
		        StrippingBd2KstarMuMu.line_10hz,  
		        StrippingBd2KstarMuMu.line_2hz,  
		        StrippingBd2KstarMuMu.line_1hz,  
		        StrippingBs2Jpsif0.line,  
		        StrippingBiasedBs2JpsiPhi.line,  
		        StrippingBu2JpsiK.line,  
		        StrippingBu2JpsiKDetached.line,  
		        StrippingBu2JpsiKDetachedR.line
		    ] )
