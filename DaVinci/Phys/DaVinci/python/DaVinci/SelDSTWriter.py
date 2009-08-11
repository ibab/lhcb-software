"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: SelDSTWriter.py,v 1.4 2009-08-11 14:50:24 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

#from AnalysisConf.BaseDSTWriter import BaseDSTWriter
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

    _propertyDocDct = {"SaveCandidates" : """Copy candidate decay tree"""}

    def outputStreamType(self) :
        from Configurables import InputCopyStream
        return InputCopyStream

    def extendStream(self, stream) :
#        stream.OptItemList = self.getProp("Items")
        stream.TakeOptionalFromTES = True
        stream.Preload = False          # True makes LoKi crash (why?)
        stream.PreloadOptItems = False  # True makes LoKi crash (why?)
        if self.getProp("SaveCandidates") :
            MicroDSTWriter.extendStream(self, stream)

    def extendSequence(self, sel) :
        if self.getProp("SaveCandidates") :
            MicroDSTWriter.extendSequence(self, sel)

