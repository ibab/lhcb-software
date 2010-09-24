"""
Default configuration parameters for selection DST.
"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('selDSTElements',
           'selDSTStreamConf')

from GaudiConf.Configuration import *
from Configurables import InputCopyStream
from streamconf import OutputStreamConf
from microdstelements import CloneParticleTrees, ClonePVRelations

def selDSTElements() :
    return[CloneParticleTrees(copyProtoParticles = False),
           ClonePVRelations("Particle2VertexRelations",True)]

def selDSTStreamConf() :
    return OutputStreamConf(streamType = InputCopyStream)
