"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: BaseDSTWriter.py,v 1.6 2009-09-15 13:22:15 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

class BaseDSTWriter(ConfigurableUser) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file, plus optional extra items from the TES.
    """
    __slots__ = {
        "OutputFileSuffix"           : ""
        , "SelectionSequences"       : []
        , "ExtraItems"               : []
        }

    _propertyDocDct = {  
        "OutputFileSuffix"             : """Add to name of output DST file. Default ''"""
        , "SelectionSequences" : """ Name of SelectionSequence that defines the selection"""
        , "ExtraItems"         : """ Extra TES locations to be written. Default: []"""
        }

    def sequence(self) :
        return GaudiSequencer(self.name() + "MainSeq",
                              IgnoreFilterPassed = True)

    def selectionSequences(self) :
        return self.getProp('SelectionSequences')

    def streamName(self, name) :
        return 'OStream' + name

    def outputStreamType(self) :
        from Configurables import InputCopyStream
        return InputCopyStream

    def extendStream(self, stream) :
        # do nothing
        return []

    def _extendStream(self, stream) :
        # Add ExtraItems
        extras = []
        extras +=  self.getProp('ExtraItems')
        stream.OptItemList = extras


    def fileExtension(self) :
        return ".dst"

    def outputFileName(self, name) :
        if name == "" : name = 'Output'
        dstName = name+"_"+self.getProp('OutputFileSuffix')+self.fileExtension()
        return "DATAFILE='" + dstName + "' TYP='POOL_ROOTTREE' OPT='REC'"
    
    def _initOutputStreams(self, name) :
        stream = self.outputStreamType()( self.streamName(name) )
        stream.Output = self.outputFileName(name)
        self._extendStream(stream)
        self.extendStream(stream)
        
    def outputStream(self, name) :
        return self.outputStreamType()( self.streamName(name) )

    def extendSequence(self, sel) :
        return
    
    def addOutputStream(self, seq) :
        outStream = self.outputStream(seq.name())
        if outStream != None :
            seq.Members += [self.outputStream(seq.name())]

    def __apply_configuration__(self) :
        """
        BaseDSTWriter configuration
        """
        log.info("Configuring BaseDSTWriter")
        for sel in self.selectionSequences() :
            seq = GaudiSequencer("Seq"+sel.name(), Members = [sel.sequence()])
            self._initOutputStreams(seq.name())
            seq.Members += self.extendSequence(sel)
            self.addOutputStream(seq)
            self.sequence().Members += [ seq ]
