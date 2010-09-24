"""

"""
__version__ = "$Id: MicroDSTWriter.py,v 1.8 2010-08-05 16:16:25 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from DSTWriters.__dev__.BaseDSTWriter import BaseDSTWriter

from DSTWriters.streamconf import microDSTWriterConf

from DSTWriters.microdstelements import  (CloneRecHeader,
                                          CloneODIN,
                                          ClonePVs,
                                          CloneParticleTrees,
                                          ClonePVRelations,
                                          CloneBackCat,
                                          CloneBTaggingInfo,
                                          ReFitAndClonePVs)

defaultElements = [CloneRecHeader(),
                   CloneODIN(),
                   ClonePVs(),
                   CloneParticleTrees(copyProtoParticles = False),
                   ClonePVRelations("Particle2VertexRelations",True)]

class MicroDSTWriter(BaseDSTWriter) :
    """
    Write a MicroDST for a set of selection sequences.
    """

    __slots__ = { "StreamConf"       : { 'default' : microDSTWriterConf() },
                  "MicroDSTElements" : { 'default' : defaultElements }

                  }
