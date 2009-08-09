"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: SelDSTWriter.py,v 1.3 2009-08-09 21:30:59 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

class SelDSTWriter(ConfigurableUser) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file, plus extra items from the TES.
    """
    __slots__ = {
        "DSTFile"           : "DaVinci.dst"
        , "SelectionSequence"    : ""
        , "Items"                : []
        }

    _propertyDocDct = {  
        "DSTFile"             : """ Write name of output MicroDST file. Default 'MicroDST.dst'"""
        , "SelectionSequence" : """ Name of PhysSlePython.SelectionSequence that defines the selection"""
        , "Items"             : """ Extra items from the TES to be copied."""
        }

    def selectionSeq(self) :
        return self.getProp('SelectionSequence')

    def _streamName(self) :
        return 'DSTStream'+ self.name()
    
    def _initOutStream(self) :
        from Configurables import InputCopyStream
        stream = InputCopyStream( self._streamName() )
        stream.OptItemList = self.getProp("Items")
        from Configurables import MicroDSTWriter
        if type (self.selectionSeq() ) == MicroDSTWriter :
            self.outputStream().OptItemList += ["/Event/" + self.selectionSeq().name() + "#99"]
        stream.TakeOptionalFromTES = True
        stream.Preload = False          # True makes LoKi crash (why?)
        stream.PreloadOptItems = False  # True makes LoKi crash (why?)
        dstName = self.getProp('DSTFile')
        if (dstName != "" ) :
            stream.Output = "DATAFILE='"+dstName +"' TYP='POOL_ROOTTREE' OPT='REC'"
        
    def outputStream(self) :
        return InputCopyStream(self._streamName())

    def sequence(self) :
        return self.selectionSeq().sequence()

    def algorithm(self) :
        return self.selectionSeq().algorithm()

    def algName(self) :        
        return self.algorithm().name()

    def __apply_configuration__(self) :
        """
        SelDSTWriter configuration
        """

        self._initOutStream()
        self.sequence().Members += [self.outputStream()]
