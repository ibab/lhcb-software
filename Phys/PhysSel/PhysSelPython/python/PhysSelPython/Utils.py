"""Set of utilities for handling and acting upon selection-type objects.

"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

from copy import copy

from SelPy.selection import FlatSelectionListBuilder   
from PhysSelPython.Wrappers import MergedSelection

def _getAlgoList(sel) :
    return copy(FlatSelectionListBuilder(sel).selectionList)

def _cloneSelection(sel, name) :
    return sel.clone(name)

def _mergedSelection(sel, name) :
    return MergedSelection(name, RequiredSelections = [sel])

def _eventSelection(sel, name) :
    if sel.outputLocation() == '' :
        return sel
    else :
        return _mergedSelection(sel, name)
    

class dummy(object) :
    '''
    This class should return a list of configurables and an output location.
    '''

    _getMemberAlgs = {'MergedSelection' : lambda x : [x.algorithm()]   }

    _getSelectionAlgs = { 'Selection'      : _cloneSelection,
                          'MergedSelection': _cloneSelection,
                          'EventSelection' : _eventSelection,
                          'AutomaticData'  : _mergedSelection  }
    
    
    def __init__(self, name, selection) :

        self._sel = self.getSelection(selection, name)
        self._members = self.getMembers(self._sel)
        self._outputLocation = self._sel.outputLocation()

    def getMembers(self, selection) :
        selTypeName = type(selection).__name__
        return dummy._getMemberAlgs.get(selTypeName, _getAlgoList)(selection)

    def getSelection(self, selection, name) :
        selTypeName = type(selection).__name__
        return dummy._getSelectionAlgs.get(selTypeName, lambda x : x)(selection, name)
    
    def outputLocation(self) :
        return self._outputLocation
    
    def members(self) :
        return copy(self._members)

    def selection(self) :
        return self._sel

def _getAlgoAsList(sel) :
    return [sel.algorithm()]

def _handleGenericSelectionType(self, name, sel) :
    self._members += _getAlgoList(sel)
    loc = sel.outputLocation()
    self._outputsel = loc
    self._outputloc = loc
    
def _handle_Selection(self, name, alg) :
    sel = alg.clone(name)
    self._handleGenericSelectionType( name, sel )

def _handle_EventSelection(self, name, alg) :
    sel = MergedSelection(name, RequiredSelections = [alg])    
    self._handleGenericSelectionType( name, sel )

def _handle_MergedSelection(self, name, alg) :
    sel = alg.clone(name)
    self._members += _getAlgoAsList(sel)
    loc = sel.outputLocation()
    self._outputsel = loc
    self._outputloc = loc
    
def _handle_AutomaticData(self, name, alg) :
    sel = MergedSelection(name, RequiredSelections = [alg])
    self._handleGenericSelectionType( name, sel )
                                                                                                                                                                                                                                        
