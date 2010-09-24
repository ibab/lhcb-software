"""
Default configuration for stripping DST.
"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('stripDSTElements',
           'stripDSTStreamConf')

from GaudiConf.Configuration import *
from Configurables import InputCopyStream

from streamconf import OutputStreamConf
from microdstelements import CloneParticleTrees, ClonePVRelations

def stripDSTElements() :
    return[CloneParticleTrees(copyProtoParticles = False),
           ClonePVRelations("Particle2VertexRelations",True)]

def stripDSTStreamConf() :
    return OutputStreamConf(streamType = InputCopyStream)
