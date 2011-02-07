"""Set of utilities for creating StrippingLine Like objects from selection-type objects.
"""

__all__ = ('MockStrippingLine')

__author__ = 'Juan Palacios palacios@physik.uzh.ch'



from copy import copy

from SelPy.selection import flatAlgorithmList
from PhysSelPython.Wrappers import MergedSelection

def _getAlgoList(sel) :
    return copy(flatAlgorithmList(sel))

def _cloneSelection(sel, name) :
    return sel.clone(name)

def _mergedSelection(sel, name) :
    return MergedSelection(name, RequiredSelections = [sel])

def _eventSelection(sel, name) :
    if sel.outputLocation() == '' :
        return sel
    else :
        return _mergedSelection(sel, name)
    

class MockStrippingLine(object) :
    """
    This class should return a list of configurables and an output location.
    Usage:
    >>> sel = Selection('Fred', ....)
    >>> print sel.name(), sel.outputLocation()
    Fred Phys/Fred
    >>> mockStrippingLineComponents = MockStrippingLine('StrippingLineFred', sel)
    >>> srtippingSel = mockStrippingLineComponents.selection()
    >>> print strippingSel.name(), strippingSel.outputLocation()
    StrippingLineFred, Phys/StrippingLineFred
    >>> # get the configurables
    >>> configs = mockStrippingLineComponents.members()
    """

    _getMemberAlgs = {'MergedSelection' : lambda x : [x.algorithm()]   }

    _getSelectionAlgs = { 'Selection'            : _cloneSelection,
                          'MergedSelection'      : _cloneSelection,
                          'EventSelection'       : _eventSelection,
                          'VoidEventSelection'   : _eventSelection,
                          'PassThroughSelection' : _eventSelection,
                          'AutomaticData'        : _mergedSelection  }
    
    
    def __init__(self, name, selection) :

        self._sel = self._getSelection(selection, name)
        self._members = self._getMembers(self._sel)
        self._outputLocation = self._sel.outputLocation()
        self._name = 'Stripping' + name
        
    def _getMembers(self, selection) :
        selTypeName = type(selection).__name__
        return MockStrippingLine._getMemberAlgs.get(selTypeName, _getAlgoList)(selection)

    def _getSelection(self, selection, name) :
        selTypeName = type(selection).__name__
        return MockStrippingLine._getSelectionAlgs.get(selTypeName, lambda x : x)(selection, name)

    def name(self) :
        return self._name
    
    def outputLocation(self) :
        return self._outputLocation
    
    def members(self) :
        return copy(self._members)

    def selection(self) :
        return self._sel

    def clone ( self , name , selection) :
        return MockStrippingLine(name, selection)

    def configurable(self) :
        return GaudiSequencer(self._name, Members = self.members())
