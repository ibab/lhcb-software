''' Helpers for DST writing.
'''
__author__ = "Juan Palacios <juan.palacios@nikhef.nl>"

__all__ = ('ConfigurableList')

class ConfigurableList(object) :
    '''
    Return a list of plain configurables from a SelectionSequence style object.
    Intended to deal with difference in interfaces of SelectionSequence, StrippingStream, etc.
    '''
    _algos = { 'SelectionSequence' : lambda x : x.__getattribute__('algs'),
               'StrippingStream' : lambda x : x.filterMembers(),
               'StrippingLine' : lambda x : x.filterMembers(),
               'StrippingConf' : lambda x : x.filterMembers()
               }
    def __init__(self, selection) :
        self._sel = selection
    def name(self) :
        return type(self._sel).__name__
    def flatList(self) :
        print 'ConfigurableList working!'
        return ConfigurableList._algos[self.name()](self._sel)
