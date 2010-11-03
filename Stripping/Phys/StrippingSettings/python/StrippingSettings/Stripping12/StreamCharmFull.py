# $Id: StreamCharm.py,v 1.4 2010-09-08 09:05:18 gcowan Exp $
#
#   Charm stripping selection for microDST
#
#   all physics channels with hadronic final states from charm decay
#   This is a copy of the lines in the Charm microDST stream, no prescales
#
#   @author Greig Cowan and Tom Blake 
#   @date 2010-09-07
# 
#   DO NOT MODIFY THIS FILE WITHOUT PERMISSION OF THE AUTHORS
#

from Gaudi.Configuration import *
from StrippingConf.StrippingStream import StrippingStream
from StrippingSettings.Stripping12.StreamCharmMicroDST import cloneableCharmLines 

stream = StrippingStream("CharmFull")
stream.appendLines( [ line.clone("Full" + line.name().lstrip("Stripping") ) for line in cloneableCharmLines ] )


# Now a big hack to get the uncloneable lines in

#from StrippingSelections.StrippingDstarPromptWithD02K3Pi import DstarPromptWithD02K3PiAllLinesConf
#DstarPromptWithD02K3PiConf = DstarPromptWithD02K3PiAllLinesConf("FullDstarPromptWithD02K3Pi")
#stream.appendLines( DstarPromptWithD02K3PiConf.Lines )

# Sebastian Bachmann (Double D0 for tag and probe)
#from StrippingSelections.StrippingX2D0D0 import StrippingX2D0D0Conf
#stream.appendLines( [ StrippingX2D0D0Conf( prefix = "Full" ).line() ] )

# Patrick Spradlin, Philip Xing ( D*-tagged yCP lines )
#from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf
#StrippingDstarPromptWithD02HHConf().prefix = 'FullDstarPrompt'
#stream.appendLines( StrippingDstarPromptWithD02HHConf().linesDstarOnly() )

# Phillip Urquijo
#from StrippingSelections.StrippingD2XMuMuSS import StrippingD2XMuMuSSConf
#from StrippingConf.StrippingStream import StrippingStream
#StrippingD2XMuMuSSConf().prefix = "Full"

#stream.appendLines( [ StrippingD2XMuMuSSConf().PiSS_line(),
#                      StrippingD2XMuMuSSConf().PiOS_line(),
#                      StrippingD2XMuMuSSConf().KSS_line(),
#                      StrippingD2XMuMuSSConf().KOS_line() ] )

# Walter Bonivento, D0 -> mu mu searches
#from StrippingSelections.StrippingDstarD02xx import  StrippingDstarD02xxConf
#stream.appendLines( StrippingDstarD02xxConf("DstarD02xx", prefix = "Full").lines() )

