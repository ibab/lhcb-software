''' '''
#$Id: dstwriters.py,v 1.6 2010-08-03 16:47:25 jpalac Exp $


__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__version__ = '$Revision: 1.6 $'


from Configurables import GaudiSequencer, InputCopyStream, OutputStream, RecordStream

from streamconf import OutputStreamConf

from copy import copy
        
baseDSTWriterConf = OutputStreamConf(streamType = InputCopyStream)

microDSTWriterConf = OutputStreamConf(streamType = OutputStream,
                                      fileExtension = '.mdst',
                                      extraItems = ['/Event/DAQ/ODIN#1',
                                                    '/Event/Rec/Header#1'])

selDSTWriterConf = OutputStreamConf(streamType = InputCopyStream)

class DSTWriterSelectionSequence(object) :
    '''
    Extends a SelectionSequence to write a DST.
    Takes a SelectionSequence as an input. Other arguments include
    the output stream configuration and a MicroDSTExtras, which
    has a list of callables that populate the relevant MicroDST
    locations plut their output location trunk.
    '''
    def __init__(self,
                 selSequence,
                 outputStreamConfiguration = baseDSTWriterConf,
                 writeFSR = True,
                 extras = None ) :

        outputStreamConfiguration.name=selSequence.name()
        self.stream = outputStream(outputStreamConfiguration)
        self.stream.OutputLevel=1
        self.fsrStream = fsrStream(outputStreamConfiguration)
        self.algos = [selSequence.sequence()]
        if writeFSR :
            self.algos.insert(0, self.fsrStream)
        self.name = "DSTWriter"+selSequence.name()
        self.mainSeq = None
        if None != extras :
            for _algs in [x(selSequence) for x in extras] :
                self.algos += _algs
            print 'XXX ADDING', extras.output, 'to stream', self.stream.name() 
            self.stream.OptItemList += [extras.output]
        self.algos.append(self.stream)

    def sequence(self, sequencerType = GaudiSequencer) :
        if self.mainSeq == None :
            self.mainSeq = sequencerType(self.name, Members = copy(self.algos))
        return self.mainSeq

def outputStream(conf) :
    '''
    Create an output stream object based in a stream configuration input.
    '''

    stream = conf.streamType('OStream'+conf.name,
                             Output = _poolFileName(conf.filePrefix + conf.name + conf.extension))
    print 'XXX adding extraitems', conf.extraItems, 'for conf name', conf.name
    stream.OptItemList += conf.extraItems
    return stream

def fsrStream(conf) :
    return RecordStream('FileRecordStream' + conf.name,
                        ItemList         = [ "/FileRecords#999" ],
                        EvtDataSvc       = "FileRecordDataSvc",
                        EvtConversionSvc = "FileRecordPersistencySvc",
                        Output           = _poolFileName(conf.filePrefix + conf.name + conf.extension)
                        )

def _poolFileName(name) :
    '''
    Add necessary elements to simple filename to make it a valid Pool output name.
    '''
    if name == "" : name = 'Output'
    return "DATAFILE='" + name + "' TYP='POOL_ROOTTREE' OPT='REC' "
