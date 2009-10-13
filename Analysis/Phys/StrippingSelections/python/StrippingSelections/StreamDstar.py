#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingDstarNoPID
from StrippingSelections import StrippingBd2DstarMuNu

stream = StrippingStream("Dstar")
stream.appendLines( [ 
		        StrippingDstarNoPID.line, 
		        StrippingBd2DstarMuNu.line
		    ] )
