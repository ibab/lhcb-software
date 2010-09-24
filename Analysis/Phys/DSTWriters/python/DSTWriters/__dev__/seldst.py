"""
Default configuration parameters for selection DST.
"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('selDSTStreamConf')

from GaudiConf.Configuration import *
from Configurables import InputCopyStream
from streamconf import OutputStreamConf

def selDSTStreamConf() :
    return OutputStreamConf(streamType = InputCopyStream)
