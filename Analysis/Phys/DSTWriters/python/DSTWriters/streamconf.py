'''Classes and functions to control the configuration of output streams used for DST writing. '''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__version__ = '$Revision: 1.3 $'

__all__ = ('OutputStreamConf', 'OutputStreamConfParams')

from copy import copy

from GaudiConf.Configuration import ConfigurableUser

class OutputStreamConf(ConfigurableUser) :
        
    __slots__ = { 'streamType' : None,
                  'filePrefix' : 'Sel',
                  'fileExtension' : '.dst',
                  'extraItems' :  []}

    def params(self) :
        return OutputStreamConfParams(streamType = self.getProp('streamType'),
                                      filePrefix = self.getProp('filePrefix'),
                                      fileExtension = self.getProp('fileExtension'),
                                      extraItems = copy(self.getProp('extraItems'))  )

class OutputStreamConfParams(object) :
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

