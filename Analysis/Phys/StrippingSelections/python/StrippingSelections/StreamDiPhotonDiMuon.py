#
#   Electroweak stripping selections 
#
#   Stream for pp -> pp mu mu events for indirect luminosity
#   measurement
#            
#
#   @author J. Keaveney
#   @date 2010-01-24
#
from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream
from StrippingSelections import  StrippingDiPhotonDiMuon

stream = StrippingStream("DiPhotonDiMuon")
stream.appendLines( [ 
                        StrippingDiPhotonDiMuon
		    ] )
