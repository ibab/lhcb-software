"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: BaseDSTWriter.py,v 1.5 2010-05-12 14:44:19 panmanj Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

class BaseDSTWriter(ConfigurableUser) :
    """
    Write a DST for a single selection sequence. Writes out the entire
    contents of the input DST file, plus optional extra items from the TES.
    """
    __slots__ = {
        "OutputFileSuffix"           : "Sel"
        , "SelectionSequences"       : []
        , "ExtraItems"               : []
        , "WriteFSR"                 : True
        }

    _propertyDocDct = {  
        "OutputFileSuffix"             : """Add to name of output DST file. Default 'Sel'"""
        , "SelectionSequences" : """ Name of SelectionSequence that defines the selection"""
        , "ExtraItems"         : """ Extra TES locations to be written. Default: []"""
        , "WriteFSR"           : """ Flags whether to write out an FSR """
        }

    def sequence(self) :
        return GaudiSequencer(self.name() + "MainSeq",
                              ModeOR = True, 
                              ShortCircuit = False)

    def selectionSequences(self) :
        return self.getProp('SelectionSequences')

    def streamName(self, name) :
        return 'OStream' + name

    def fsrStreamName(self, name) :
        return 'FileRecordStream' + name

    def outputStreamType(self) :
        from Configurables import InputCopyStream
        return InputCopyStream

    def extendStream(self, seq, stream) :
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
        dstName = self.getProp('OutputFileSuffix')+name+self.fileExtension()
        return "DATAFILE='" + dstName + "' TYP='POOL_ROOTTREE' OPT='REC' "
    
    def _initOutputStreams(self, seq) :
        name = seq.name()
        stream = self.outputStreamType()( self.streamName(name) )
        stream.Output = self.outputFileName(name)
        self._extendStream(stream)
        self.extendStream(seq, stream)
        
    def outputStream(self, name) :
        return self.outputStreamType()( self.streamName(name) )
        
    def fsrOutputStream(self, name) :
        """
        write out the FSR

        for this to work the main configurable, e.g. DaVinci must also set WriteFSR to True
        """
        fsrStreamName = self.fsrStreamName(name)
        dstName = self.getProp('OutputFileSuffix')+name+self.fileExtension()
        # Output stream to the same file
        FSRWriter = RecordStream( fsrStreamName,
                                  ItemList         = [ "/FileRecords#999" ],
                                  EvtDataSvc       = "FileRecordDataSvc",
                                  Output           = "DATAFILE='"+dstName+"' TYP='POOL_ROOTTREE'  OPT='REC'",
                                  )

        # Write the FSRs to the same file as the events
        return FSRWriter 

    def extendSequence(self, sel) :
        return []
    
    def addOutputStream(self, seq) :
        # FSR stream first
        if self.getProp('WriteFSR'):
            fsrStream = self.fsrOutputStream(seq.name())
            if fsrStream != None :
                seq.Members = [ fsrStream ] + seq.Members
        # event data stream
        outStream = self.outputStream(seq.name())
        if outStream != None :
            seq.Members += [self.outputStream(seq.name())]

    def __apply_configuration__(self) :
        """
        BaseDSTWriter configuration
        """
        log.info("Configuring BaseDSTWriter")
        for sel in self.selectionSequences() :
            seq = GaudiSequencer("."+sel.name(), Members = [sel.sequence()],
                                 MeasureTime=True)
            self._initOutputStreams(seq)
            seq.Members += self.extendSequence(sel)
            self.addOutputStream(seq)
            self.sequence().Members += [ seq ]
