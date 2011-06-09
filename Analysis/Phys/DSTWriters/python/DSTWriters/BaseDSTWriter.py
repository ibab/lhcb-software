"""
Write a DST for a single selection sequence. Writes out the entire
contents of the input DST
"""
__version__ = "$Id: BaseDSTWriter.py,v 1.12 2010-08-12 11:15:53 jpalac Exp $"
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

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
        , "RootInTES"                : '/Event/'
        , "Persistency"              : None
        }

    _propertyDocDct = {  
        "OutputFileSuffix"             : """Add to name of output DST file. Default 'Sel'"""
        , "SelectionSequences" : """ Name of SelectionSequence that defines the selection"""
        , "ExtraItems"         : """ Extra TES locations to be written. Default: []"""
        , "WriteFSR"           : """ Flags whether to write out an FSR """
        , "RootInTES"          : """ RootInTES property for sequences and MicroDST output location."""
        , "Persistency"        : """ Overwrite the default persistency with this persistency. """
        }
    
    def sequence(self) :
        return GaudiSequencer(self.name() + "MainSeq",
                              ModeOR = True, 
                              ShortCircuit = False,
                              RootInTES = self.getProp('RootInTES'))
    
    def selectionSequences(self) :
        return self.getProp('SelectionSequences')
    
    def streamName(self, name) :
        if name[0] == '.' :
            name = name[1:]
        return name + '_OStream'
    
    #def fsrStreamName(self, name) :
    #    return 'FileRecordStream' + name
    
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
        return self.getProp('OutputFileSuffix')+name+self.fileExtension()
    
    #def outputFileName(self, name) :
    #    dstName=self._undressedName(name)
    #    from GaudiConf import IOHelper
    #    return IOHelper().dressFile(dstName,"O")
    
    def _initOutputStreams(self, seq) :
        name = seq.name()
        stream = self.outputStream(name)
        stream.OutputLevel = 3
        self._extendStream(stream)
        from GaudiConf import IOHelper
        dstName = self.outputFileName(name)
        self.extendStream(seq, stream)
        ioh=None
        if hasattr(self, "Persistency"):
            persistency=self.getProp("Persistency")
            ioh=IOHelper(persistency,persistency)
        else:
            ioh=IOHelper()
        return ioh.outputAlgs(filename=dstName, writer=stream, writeFSR=self.getProp('WriteFSR'))
        #stream.Output = self.outputFileName(name)
        #self._extendStream(stream)
        #self.extendStream(seq, stream)
    
    def outputStream(self, name) :
        return self.outputStreamType()( self.streamName(name) )
    
    #def fsrOutputStream(self, name) :
    #    """
    #    write out the FSR
    #    
    #    for this to work the main configurable, e.g. DaVinci must also set WriteFSR to True
    #    """
    #    fsrStreamName = self.fsrStreamName(name)
    #    dstName = self.getProp('OutputFileSuffix')+name+self.fileExtension()
    #    # Output stream to the same file
    #    FSRWriter = RecordStream( fsrStreamName,
    #                              ItemList         = [ "/FileRecords#999" ],
    #                              EvtDataSvc       = "FileRecordDataSvc",
    #                              EvtConversionSvc = "FileRecordPersistencySvc",
    #                              Output           = "DATAFILE='"+dstName+"' TYP='POOL_ROOTTREE'  OPT='REC'"
    #                              )
    #
    #    # Write the FSRs to the same file as the events
    #    return FSRWriter 
    
    def extendSequence(self, sel) :
        return []
    
    def addOutputStream(self, seq, algs) :
        # FSR stream first
        print 'BaseDSTWriter.addOutputStream'
        from GaudiConf import IOHelper
        for alg in algs:
            if IOHelper().detectStreamType(alg) in ["FSR"]:
                #FSR stream
                if self.getProp('WriteFSR'):
                    seq.Members = [ alg ] + seq.Members
            else:
                #event data stream
                seq.Members += [alg]
    
    def __apply_configuration__(self) :
        """
        BaseDSTWriter configuration
        """
        log.info("Configuring BaseDSTWriter")
        #log.info("OMG Rob in apply of BaseDSTWriter")
        from Configurables import FixInputCopyStream
        
        for sel in self.selectionSequences() :
            seq = GaudiSequencer("."+sel.name(), Members = [sel.sequence()],
                                 MeasureTime=True)
            #print log.info("OMG Rob ready to initOutputStreams")
            algs=self._initOutputStreams(seq)
            #print log.info("OMG Rob algs are"+ str(algs))
            seq.Members += self.extendSequence(sel)
            #print log.info("OMG Rob seq.Members are"+ str(seq.Members))
            seq.Members += [FixInputCopyStream()]
            #print log.info("Added fixing, OMG Rob seq.Members are"+str(seq.Members) + "algs are"+str(algs))
            self.addOutputStream(seq=seq, algs=algs)
            #print log.info("OMG Rob Added output streams, seq.Members are"+str(seq.Members))
            self.sequence().Members += [ seq ]
