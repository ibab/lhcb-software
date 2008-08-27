from LbUtils.CMT import Graph
from LbProject import LbProject


class LbGraph(Graph):
    def __init__(self, projectpath):
        super(LbGraph, self).__init__(projectpath)
        self._prjclass = LbProject
