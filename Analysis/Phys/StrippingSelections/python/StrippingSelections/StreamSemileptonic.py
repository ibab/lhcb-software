#
#   Stripping selections 
#
#   B (and charm) decays with one muon required, exclusive and
#   semi-inclusive selections.
#
#   @author A. Poluektov
#   @date 2009-05-15
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream

from StrippingSelections import StrippingBd2DstarMuNu
from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
from StrippingSelections import StrippingBsDsMuNu
from StrippingSelections import Strippingb2DMuX

stream = StrippingStream("Semileptonic")
stream.appendLines( [ 
		        StrippingBd2DstarMuNu.line, 
		        StrippingBd2DstarMuNuLooseConf().line(), 
		        StrippingBsDsMuNu.line, 
			Strippingb2DMuX.line, 
			Strippingb2DMuX.line1
		    ] )
