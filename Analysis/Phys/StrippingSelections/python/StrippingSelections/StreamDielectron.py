#
#   Dimuon stripping selections 
# 
#   all physics channels with at least 2 electrons inside
#   (with minimal electron ID required)
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingBd2Kstaree
from StrippingSelections.StrippingBu2LLK  import StrippingBu2LLKConf
from StrippingSelections.StrippingJpsi2ee  import StrippingDiElectronConf

stream = StrippingStream("Dielectron")
stream.appendLines( [ 
		        StrippingBd2Kstaree.line ,
		        StrippingBu2LLKConf().eeK(),
                        StrippingDiElectronConf().loose_line()
		    ] )
