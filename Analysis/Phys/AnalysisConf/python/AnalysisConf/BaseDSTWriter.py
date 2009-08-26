"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: BaseDSTWriter.py,v 1.4 2009-08-26 16:16:59 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

class BaseDSTWriter(ConfigurableUser) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file.
    """
    __slots__ = {
        "OutputFilePrefix"           : ""
        , "SelectionSequences"       : []
        }

    _propertyDocDct = {  
        "OutputFilePrefix"             : """ Write name of output MicroDST file. Default 'MicroDST.dst'"""
        , "SelectionSequences" : """ Name of PhysSlePython.SelectionSequence that defines the selection"""
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

    def fileExtension(self) :
        return ".dst"

    def outputFileName(self, name) :
        dstName = self.getProp('OutputFilePrefix')+name+self.fileExtension()
        return "DATAFILE='" + dstName + "' TYP='POOL_ROOTTREE' OPT='REC'"
    
    def _initOutputStreams(self, name) :
        stream = self.outputStreamType()( self.streamName(name) )
        stream.Output = self.outputFileName(name)
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
