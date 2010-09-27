"""Add python dictionary methods to DaVinci::Map maps.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

__all__ = ()

import GaudiPython

from GaudiPython.Pythonizations import (__mapbase_iteritems__,
                                        __mapbase_iter__,
                                        __mapbase_keys__,
                                        __mapbase_items__,
                                        __mapbase_values__,
                                        __mapbase_contains__,
                                        __mapbase_get__,
                                        __mapbase_str__,
                                        __mapbase_setitem__,
                                        __mapbase_delitem__)

_maps = (GaudiPython.gbl.DaVinci.Map.Particle2VertexBase,
         GaudiPython.gbl.DaVinci.Map.Particle2LHCbIDs,
         GaudiPython.gbl.DaVinci.Map.Particle2UnsignedInts)

def _addPythonMembers(_map) :
    _map.__len__       = lambda s   : s.size()
    _map.__iter__      = __mapbase_iter__ 
    _map.  keys        = __mapbase_keys__
    _map.__iteritems__ = __mapbase_iteritems__
    _map. iteritems    = __mapbase_iteritems__
    _map. items        = __mapbase_items__
    _map. values       = __mapbase_values__
    _map.__contains__  = __mapbase_contains__
    _map. has_key      = __mapbase_contains__
    _map. get          = __mapbase_get__
    _map.__str__       = __mapbase_str__
    _map.__repr__      = __mapbase_str__ 
    _map.__setitem__   = __mapbase_setitem__
    _map.__delitem__   = __mapbase_delitem__ 
    _map.__getitem__   = lambda s,key : s.at ( key )


for _map in _maps :
    _addPythonMembers(_map)

