'''Classes and functions to control the configuration of output streams used for DST writing. '''

__author__ = "Juan PALACIOS juan.palacios@nikhef.nl"

__version__ = '$Revision: 1.2 $'

__all__ = ('OutputStreamConf')

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

