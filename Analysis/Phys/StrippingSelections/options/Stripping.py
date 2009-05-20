#
#   Stripping main options. Just import this.
#
#   @author P. Koppenburg
#   @date 2009-05-20
#
from Gaudi.Configuration import *
from StrippingConf.Configuration import StrippingConf


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
importOptions("$STRIPPINGSELECTIONSROOT/options/StrippingSelections.py")
StrippingConf().ActiveLines = []                      # Specify the active stripping lines
                                                      # If empty (default), all defined StrippingLines will be active

StrippingConf().ActiveStreams = []                    # Specify the active streams. Only selections from 
                                                      # these streams will run. If empty, all streams are active

StrippingConf().OutputType = "DST"                    # Can be either "ETC" or "DST"
