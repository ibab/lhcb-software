'''Classes and functions to control the configuration of output streams used for DST writing. '''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__all__ = ('OutputStreamConf',
           'defaultDSTWriterConf',
           'selDSTWriterConf',
           'microDSTWriterConf',
           'stripDSTWriterConf')

from copy import copy

class OutputStreamConf(object) :
    def __init__(self,
                 streamType = None,
                 filePrefix = 'Sel',
                 fileExtension = '.dst',
                 extraItems = []) :
        self.streamType = streamType
        self.filePrefix = filePrefix
        self.extension = fileExtension
        self.extraItems = copy(extraItems)
        self.name = ''    

    def __str__(self) :
        output = '\nOutputStreamConfXX\n'
        output += 'name       : '+ self.name + '\n'
        output += 'streamType : '+ str(self.streamType) + '\n'
        output += 'filePrefix : '+ str(self.filePrefix) + '\n'
        output += 'extension  : '+ str(self.extension) + '\n'
        output += 'extraItems : '+ str(self.extraItems) + '\n'
        return output

def defaultDSTWriterConf():
    return OutputStreamConf(streamType = InputCopyStream)

def microDSTWriterConf() :
    return OutputStreamConf(streamType = OutputStream,
                            fileExtension = '.mdst',
                            extraItems = ['/Event/DAQ/ODIN#1',
                                          '/Event/Rec/Header#1'])

def selDSTWriterConf() :
    return OutputStreamConf(streamType = InputCopyStream)

def stripDSTWriterConf() :
    return OutputStreamConf(streamType = InputCopyStream,
                            extraItems = [''])
