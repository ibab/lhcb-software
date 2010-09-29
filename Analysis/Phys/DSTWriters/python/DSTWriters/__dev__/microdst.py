"""

"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('microDSTElements',
           'microDSTStreamConf')

from GaudiConf.Configuration import *
from Configurables import OutputStream

from streamconf import OutputStreamConf

from microdstelements import  (CloneRecHeader,
                               CloneODIN,
                               ClonePVs,
                               CloneParticleTrees,
                               ClonePVRelations,
                               CloneBackCat,
                               CloneBTaggingInfo,
                               ReFitAndClonePVs)

def microDSTElements() :
    return [CloneRecHeader(),
            CloneODIN(),
            ClonePVs(),
            CloneParticleTrees(copyProtoParticles = True),
            ClonePVRelations("Particle2VertexRelations",True)]

def microDSTStreamConf() :
    return OutputStreamConf(streamType = OutputStream,
                            fileExtension = '.mdst',
                            extraItems = ['/Event/DAQ/ODIN#1',
                                          '/Event/Rec/Header#1'])
