"""
Default configuration for stripping DST.
"""

__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('stripDSTElements',
           'stripDSTStreamConf')

from Configurables import InputCopyStream
from streamconf import OutputStreamConf
from microdstelements import ( CloneParticleTrees, ClonePVRelations,
                               PackStrippingReports, PackParticlesAndVertices,
                               PackRecObjects, CleanEmptyEventNodes )

def stripDSTElements(pack=False) :
    elements = [ CloneParticleTrees( ProtoParticleConer = "NONE" ),
                 ClonePVRelations("Particle2VertexRelations",True),
                 ]
    if pack :
        elements += [ PackStrippingReports(),
                      PackParticlesAndVertices(),
                      PackRecObjects(),
                      CleanEmptyEventNodes() ]
    return elements

def stripDSTStreamConf() :
    return OutputStreamConf( streamType = InputCopyStream,
                             extraItems = ['/Event/DAQ/RawEvent#1'] )
##                             extraItems = ['/Event/DAQ/RawEvent#1',
##                                           '/Event/Strip/Phys/DecReports#1'])
