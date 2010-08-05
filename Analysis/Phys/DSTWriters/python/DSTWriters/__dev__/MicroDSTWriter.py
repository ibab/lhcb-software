"""

"""
__version__ = "$Id: MicroDSTWriter.py,v 1.7 2010-08-05 14:40:47 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from DSTWriters.__dev__.BaseDSTWriter import BaseDSTWriter

from DSTWriters.dstwriters import microDSTWriterConf

from DSTWriters.microdstelements import  (CloneRecHeader,
                                          CloneODIN,
                                          ClonePVs,
                                          CloneParticleTrees,
                                          ClonePVRelations,
                                          CloneBackCat,
                                          CloneBTaggingInfo,
                                          ReFitAndClonePVs)

class MicroDSTWriter(BaseDSTWriter) :
    """
    Write a MicroDST for a set of selection sequences.
    """

    __slots__ = { "StreamConf"           : microDSTWriterConf(),
                  "MicroDSTElements" : [CloneRecHeader(),
                                        CloneODIN(),
                                        ClonePVs(),
                                        CloneParticleTrees(copyProtoParticles = False),
                                        ClonePVRelations("Particle2VertexRelations",True),
                                        CloneBTaggingInfo()]
                  }
