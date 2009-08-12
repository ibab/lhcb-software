"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: SelDSTWriter.py,v 1.5 2009-08-12 07:46:31 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from MicroDSTConf.MicroDSTWriter import MicroDSTWriter

class SelDSTWriter(MicroDSTWriter) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file, plus extra items from the TES.
    """
    __slots__ = { "SaveCandidates" : False,
                  "CopyODIN"       : False,
                  "CopyRecHeader"  : False,
                  "CopyPVs"        : False,
                  "CopyBTags"      : False,
                  "OutputPrefix"   : "Sel"
                  }

    _propertyDocDct = {"SaveCandidates" : """Copy candidate decay tree""",
                       "CopyODIN" : """Copy ODIN bank into /Event/OutputPrefix """,
                       "CopyRecHeader" : """Copy RecHeader bank into /Event/OutputPrefix """,
                       "CopyPVs" : """ Copy primary vertices bank into /Event/OutputPrefix """,
                       "CopyBTags" : """ Copy FlavourTags bank into /Event/OutputPrefix """,
                       "OutputPrefix" : """ Prefix of TES location of candidates output: /Event/OutputPrefix/xxxx"""}

    def outputStreamType(self) :
        from Configurables import InputCopyStream
        return InputCopyStream

    def extendStream(self, stream) :
        stream.TakeOptionalFromTES = True
        stream.Preload = False          # True makes LoKi crash (why?)
        stream.PreloadOptItems = False  # True makes LoKi crash (why?)
        if self.getProp("SaveCandidates") :
            MicroDSTWriter.extendStream(self, stream)

    def extendSequence(self, sel) :
        if self.getProp("SaveCandidates") :
            MicroDSTWriter.extendSequence(self, sel)

