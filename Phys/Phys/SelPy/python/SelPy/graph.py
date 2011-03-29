"""Module for selection vizualisation tools.
"""

__all__ = ('graphEdges',
           'GraphEdges',
           'dot',
           'view')

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import pydot

class GraphEdges(object) :
    """
    Return a list of all the edges of the selection tree for a given
    selection object.
    """
    def __init__(self) :
        self._edges = []
    def __call__(self, selection) :
        self._edges = []
        self._getEdges(selection)
        return self._edges
    def _getEdges(self, selection) :
        for sel in selection.requiredSelections() :
            if (sel.name(), selection.name()) not in self._edges :
                self._edges.append((sel.name(), selection.name()))
            self._getEdges(sel)

graphEdges = GraphEdges()

def dot(selection) :
    """
    Return a pydot.Dot object made from the edges of selection's graph.
    """
    return pydot.graph_from_edges(graphEdges(selection), directed=True)

def view(selection) :
    """
    View the graph for a Selection type object.
    Example:
    sel = Selection ( ... )
    view(sel)
    """
    import os
    dt = dot(selection)
    if (dt.write('tmp.jpg', format='jpeg')) :
        os.system( 'display tmp.jpg &')
    else :
        print 'Error producing graph for', selection.name()
