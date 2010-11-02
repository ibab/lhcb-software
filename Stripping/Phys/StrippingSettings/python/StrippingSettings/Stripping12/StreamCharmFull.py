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
from StrippingSettings.Stripping12.StreamCharmMicroDST import stream as microDSTstream 

stream = StrippingStream("CharmFull")
#stream.appendLines( microDSTstream.lines )

