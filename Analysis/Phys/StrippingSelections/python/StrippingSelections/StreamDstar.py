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
from StrippingSelections import StrippingDstarD2KShh
from StrippingSelections import StrippingDstarD2KShhRobust
from StrippingSelections import StrippingBsDsMuNu
from StrippingSelections import Strippingb2DMuX

stream = StrippingStream("Dstar")
stream.appendLines( [ 
		        StrippingDstarNoPID.line, 
		        StrippingBd2DstarMuNu.line, 
		        StrippingDstarD2KShh.line1,
		        StrippingDstarD2KShh.line2,
		        StrippingDstarD2KShh.line3,
		        StrippingDstarD2KShh.line4,
		        StrippingDstarD2KShh.line5,
		        StrippingDstarD2KShh.line6,
		        StrippingDstarD2KShh.line7,
		        StrippingDstarD2KShh.line8,
		        StrippingDstarD2KShh.line9,
		        StrippingDstarD2KShh.line10,
		        StrippingDstarD2KShh.line11,
		        StrippingDstarD2KShh.line12,
		        StrippingDstarD2KShhRobust.line1, 
		        StrippingDstarD2KShhRobust.line2, 
		        StrippingDstarD2KShhRobust.line3, 
		        StrippingDstarD2KShhRobust.line4, 
		        StrippingDstarD2KShhRobust.line5, 
		        StrippingDstarD2KShhRobust.line6, 
		        StrippingDstarD2KShhRobust.line7, 
		        StrippingDstarD2KShhRobust.line8, 
		        StrippingDstarD2KShhRobust.line9, 
		        StrippingDstarD2KShhRobust.line10, 
		        StrippingDstarD2KShhRobust.line11, 
		        StrippingDstarD2KShhRobust.line12, 
		        StrippingBsDsMuNu.line, 
			Strippingb2DMuX.line, 
			Strippingb2DMuX.line1
		    ] )
