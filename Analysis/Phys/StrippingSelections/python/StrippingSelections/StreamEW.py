#
#   Electroweak stripping selections 
#
#            
#
#   @author J. Keaveney
#   @date 2010-01-24
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingZMuMu
from StrippingSelections import StrippingWMu
from StrippingSelections import StrippingDYMuMu
from StrippingSelections import StrippingZee
from StrippingSelections import StrippingWe
from StrippingSelections import StrippingDYee


stream = StrippingStream("EW")
stream.appendLines( [ 
		        StrippingZMuMu.line, 
		        StrippingWMu.line,  
		        StrippingDYMuMu.line1,
                        StrippingDYMuMu.line2,
		        StrippingDYMuMu.line3,
		        StrippingDYMuMu.line4,
                        StrippingZee.line,
                        StrippingWe.line,
                        StrippingDYee.line1,
                        StrippingDYee.line2,
		        StrippingDYee.line3,
		        StrippingDYee.line4, 
		    ] )
