"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: BaseDSTWriter.py,v 1.1 2009-08-11 07:46:30 jpalac Exp $"
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

    def streamName(self, sel) :
        return 'OStream' + sel.name()

    def outputStreamType(self) :
        from Configurables import InputCopyStream
        return InputCopyStream
    
    def extendStream(self, stream) :
        # do nothing
        return

    def fileExtension(self) :
        return ".dst"

    def outputFileName(self, sel) :
        dstName = self.getProp('OutputFilePrefix')+sel.name()+self.fileExtension()
        return "DATAFILE='" + dstName + "' TYP='POOL_ROOTTREE' OPT='REC'"
    
    def _initOutputStreams(self, sel) :
        if self.getProp('OutputFilePrefix') == "" :
            print "No OutputStreams!"
            return 
        stream = self.outputStreamType()( self.streamName(sel) )
        stream.Output = self.outputFileName(sel)
        self.extendStream(stream)
        
    def outputStream(self, sel) :
        if (self.getProp('OutputFilePrefix') ) != '' :
            return self.outputStreamType()( self.streamName(sel) )
        else :
            print "No OutputStream! Returning None!"

    def extendSequence(self, sel) :
        outStream = self.outputStream(sel)
        if outStream != None :
            sel.sequence().Members += [self.outputStream(sel)]

    def addOutputStream(self, sel) :
        outStream = self.outputStream(sel)
        if outStream != None :
            sel.sequence().Members += [self.outputStream(sel)]

    def __apply_configuration__(self) :
        """
        BaseDSTWriter configuration
        """
        for sel in self.selectionSequences() :
            self._initOutputStreams(sel)
            self.extendSequence(sel)
            self.addOutputStream(sel)
            self.sequence().Members += [ sel.sequence() ]
