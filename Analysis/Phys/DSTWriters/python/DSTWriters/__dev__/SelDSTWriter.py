"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: SelDSTWriter.py,v 1.4 2010-08-05 10:14:53 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from DSTWriters.__dev__.MicroDSTWriter import MicroDSTWriter
from DSTWriters.dstwriters import selDSTWriterConf


from DSTWriters.microdstelements import CloneParticleTrees, ClonePVRelations

class SelDSTWriter(MicroDSTWriter) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file, plus extra items from the TES.
    """
    __slots__ = { "StreamConf"           : selDSTWriterConf(),
                  "MicroDSTElements" : [CloneParticleTrees(copyProtoParticles = False),
                                        ClonePVRelations("Particle2VertexRelations",True)]
                  }
