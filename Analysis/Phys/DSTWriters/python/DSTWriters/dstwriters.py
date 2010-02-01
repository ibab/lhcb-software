#$Id: dstwriters.py,v 1.1 2010-02-01 21:06:05 jpalac Exp $
from Configurables import GaudiSequencer, InputCopyStream

from copy import copy

class OutputStreamConf(object) :
    pass

baseDSTWriterConf = OutputStreamConf()
baseDSTWriterConf.streamType = InputCopyStream
baseDSTWriterConf.filePrefix = 'Sel'
baseDSTWriterConf.extraItems = []

class SelectionSequenceDSTWriter(object) :
    '''
    Extends a SelectionSequence to write a DST.
    '''
    def __init__(self,
                 selSequence,
                 outputStreamConfiguration = baseDSTWriterConf,
                 sequencerType = GaudiSequencer) :

        self.seqType = sequencerType
        outputStreamConfiguration.name=selSequence.name()
        stream = outputStream(outputStreamConfiguration)
        self.selSeq = selSequence.clone("."+selSequence.name(),
                                        PostSelectionAlgs = [stream])
    def sequence(self) :
        return self.selSeq.sequence()

def outputStream(conf) :
    stream = conf.streamType(_streamName(conf.name),
                             OptItemList = conf.extraItems,
                             Output = _outputFileName(conf.filePrefix + conf.name + '.dst'))
    return stream

def _streamName(name) :
    return 'OStream' + name

def _outputFileName(name) :
    if name == "" : name = 'Output'
    return "DATAFILE='" + name + "' TYP='POOL_ROOTTREE' OPT='REC' "
