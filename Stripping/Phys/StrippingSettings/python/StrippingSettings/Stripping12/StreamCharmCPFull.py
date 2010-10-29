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
from StrippingSettings.Stripping11.StreamCharm import CharmLines

stream = StrippingStream("CharmCPFull")
stream.appendLines( CharmLines )

from StrippingSelections.StrippingDstarPromptWithD02HH import StrippingDstarPromptWithD02HHConf
stream.appendLines( StrippingDstarPromptWithD02HHConf('DstarPromptWithD02HHConfStream').linesDstarOnly() )

from StrippingSelections.StrippingDstarD02xx import  StrippingDstarD02xxConf
#CharmLines += StrippingDstarD02xxConf().lines()
stream.appendLines( StrippingDstarD02xxConf("DstarD02xx",LinePrefix="").lines() )
