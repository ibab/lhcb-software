"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: SelDSTWriter.py,v 1.12 2010-08-02 11:31:41 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

from MicroDSTWriter import MicroDSTWriter

class SelDSTWriter(MicroDSTWriter) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file, plus extra items from the TES.
    """
    __slots__ = { "SaveCandidates"     : True,
                  "CopyODIN"           : False,
                  "CopyRecHeader"      : False,
                  "CopyRecSummary"     : False,
                  "CopyPVs"            : False,
                  "CopyProtoParticles" : False,
                  "CopyBTags"          : False,
                  "CopyPVRelations"    : {"Particle2VertexRelations":True},
                  }

    _propertyDocDct = {"SaveCandidates" : """Copy candidate decay tree. Default: True.""",
                       "CopyODIN" : """Copy ODIN bank into /Event/OutputPrefix. Default: False. """,
                       "CopyRecHeader" : """Copy RecHeader bank into /Event/OutputPrefix """,
                       "CopyRecSummary" : """Copy RecSummary object into /Event/OutputPrefix """,
                       "CopyPVs" : """ Copy primary vertices bank into /Event/OutputPrefix. Default: False. """,
                       "CopyBTags" : """ Copy FlavourTags bank into /Event/OutputPrefix. Default: False. """
                       }

    def multiSequences(self) :
        return len(self.selectionSequences()) > 1

    def outputStreamType(self) :
        from Configurables import InputCopyStream
        return InputCopyStream

    def extendStream(self, seq, stream) :
#        if self.multiSequences() :
#            self.setProp('OutputPrefix', 'SequenceName')
#            log.info('More than one SelectionSequence. Partition output TES structure.')
        stream.Preload = False          # True makes LoKi crash (why?)
        stream.PreloadOptItems = False  # True makes LoKi crash (why?)
        if self.getProp("SaveCandidates") :
            MicroDSTWriter.extendStream(self, seq, stream)

    def fileExtension(self) :
        return ".dst"

    def _nodeKiller(self, seq) :
        from Configurables import EventNodeKiller
        loc = '/Event/'+self.outputPrefix(seq)
        log.info('Add Killer for node '+ loc)
        print 'Add Killer for node', loc
        return EventNodeKiller(seq.name()+'NodeKiller',
                               Nodes = [loc])

    def addOutputStream(self, seq) :
        MicroDSTWriter.addOutputStream(self, seq)
#        if self.getProp("SaveCandidates") and self.multiSequences() :
#            seq.Members += [self._nodeKiller(seq)]

    def extendSequence(self, sel) :
        seqExtension = []
        if self.getProp("SaveCandidates") :
            seqExtension+= MicroDSTWriter.extendSequence(self, sel)
        return seqExtension
