#
#   Stripping selections 
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingBd2Kstaree
from StrippingSelections.StrippingBu2LLK  import StrippingBu2LLKConf

stream = StrippingStream("Dielectron")
stream.appendLines( [ 
		        StrippingBd2Kstaree.line ,
		        StrippingBu2LLKConf().eeK()
		    ] )
