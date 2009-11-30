"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: SelDSTWriter.py,v 1.1.1.1 2009-11-30 16:37:19 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from MicroDSTConf.MicroDSTWriter import MicroDSTWriter

class SelDSTWriter(MicroDSTWriter) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file, plus extra items from the TES.
    """
    __slots__ = { "SaveCandidates"     : True,
                  "CopyODIN"           : False,
                  "CopyRecHeader"      : False,
                  "CopyPVs"            : False,
                  "CopyProtoParticles" : False,
                  "CopyBTags"          : False,
                  "CopyPVRelations"    : {"Particle2VertexRelations":False},
                  "OutputPrefix"       : "Sel"
                  }

    _propertyDocDct = {"SaveCandidates" : """Copy candidate decay tree. Default: True.""",
                       "CopyODIN" : """Copy ODIN bank into /Event/OutputPrefix. Default: False. """,
                       "CopyRecHeader" : """Copy RecHeader bank into /Event/OutputPrefix """,
                       "CopyPVs" : """ Copy primary vertices bank into /Event/OutputPrefix. Default: False. """,
                       "CopyBTags" : """ Copy FlavourTags bank into /Event/OutputPrefix. Default: False. """,
                       "OutputPrefix" : """ Prefix of TES location of candidates output: /Event/OutputPrefix/xxxx. Default 'Sel'."""}

    def outputStreamType(self) :
        from Configurables import InputCopyStream
        return InputCopyStream

    def extendStream(self, stream) :
        stream.TakeOptionalFromTES = True
        stream.Preload = False          # True makes LoKi crash (why?)
        stream.PreloadOptItems = False  # True makes LoKi crash (why?)
        if self.getProp("SaveCandidates") :
            MicroDSTWriter.extendStream(self, stream)

    def fileExtension(self) :
        return ".dst"

    def extendSequence(self, sel) :
        seqExtension = []
        if self.getProp("SaveCandidates") :
            seqExtension+= MicroDSTWriter.extendSequence(self, sel)
        return seqExtension
