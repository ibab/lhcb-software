"""Module for selection vizualisation tools.
"""

__all__ = ('graphEdges',
           'GraphEdges',
           'selectionDot')

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
            self._edges.append((selection.name(), sel.name()))
            self._getEdges(sel)

graphEdges = GraphEdges()

def selectionDot(selection) :
    """
    Return a pydot.Dot object made from the edges of selection's graph.
    """
    return pydot.graph_from_edges(graphEdges(selection))

