''' '''
#$Id: dstwriters.py,v 1.2 2010-02-02 17:30:43 jpalac Exp $


__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__version__ = '$Revision: 1.2 $'


from Configurables import GaudiSequencer, InputCopyStream, OutputStream

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
                 extras = None ) :

        outputStreamConfiguration.name=selSequence.name()
        stream = outputStream(outputStreamConfiguration)
        self.algos = copy(selSequence.algos)
        self.name = "DSTWriter"+selSequence.name()
        self.mainSeq = None
        if None != extras :
            for _algs in [x(selSequence) for x in extras] :
                self.algos += _algs
            stream.OptItemList += [extras.output]
        self.algos.append(stream)

    def sequence(self, sequencerType = GaudiSequencer) :
        if self.mainSeq == None :
            self.mainSeq = sequencerType(self.name, Members = copy(self.algos))
        return self.mainSeq

def outputStream(conf) :
    '''
    Create an output stream object based in a stream configuration input.
    '''
    stream = conf.streamType('OStream'+conf.name,
                             OptItemList = conf.extraItems,
                             Output = _poolFileName(conf.filePrefix + conf.name + conf.extension))
    try :
        stream.TakeOptionalFromTES = True
    except AttributeError :
        pass
    return stream

def _poolFileName(name) :
    '''
    Add necessary elements to simple filename to make it a valid Pool output name.
    '''
    if name == "" : name = 'Output'
    return "DATAFILE='" + name + "' TYP='POOL_ROOTTREE' OPT='REC' "
