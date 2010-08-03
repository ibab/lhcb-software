"""

"""
__version__ = "$Id: MicroDSTWriter.py,v 1.2 2010-08-03 16:47:36 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from DSTWriters.__dev__.BaseDSTWriter import BaseDSTWriter

from DSTWriters.dstwriters import microDSTWriterConf

from DSTWriters.microdstelements import  (CloneRecHeader,
                                          CloneODIN,
                                          ClonePVs,
                                          CloneParticleTrees,
                                          ClonePVRelations,
                                          CloneMCInfo,
                                          CloneBackCat,
                                          CloneBTaggingInfo,
                                          MicroDSTExtras)

class MicroDSTWriter(BaseDSTWriter) :
    """
    Write a MicroDST for a set of selection sequences.
    """

    __slots__ = { "StreamConf"           : microDSTWriterConf,
                  "MicroDSTElements" : [CloneRecHeader(),
                                        CloneODIN(),
                                        ClonePVs(),
                                        CloneParticleTrees(copyProtoParticles = False),
                                        ClonePVRelations({"Particle2VertexRelations":True}),
                                        CloneBTaggingInfo(),
                                        CloneMCInfo()]
                  }
