#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingBd2Kstaree

stream = StrippingStream("Belectron")
stream.appendLines( [ 
		        StrippingBd2Kstaree.line
		    ] )
