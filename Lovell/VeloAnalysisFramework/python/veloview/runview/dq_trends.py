"""Methods dealing with DQ values."""

from collections import OrderedDict
import ROOT

import veloview.analysis
from veloview.config import Config
from veloview.giantrootfile.gui_tree import Tree

def get_trending_variables(expert = False):
    dqVarsParser = DqVarsParser(True, True, expert)
    return dqVarsParser.get_dq_variables()

def get_dq_vars():
    return DqVarsParser().get_dq_variables()

def get_dq_values(run):
    f = ROOT.TFile(Config().grf_file_path, 'READ')
    t = Tree(Config().grf_tree_name)

    maxEntry = t.GetEntriesFast()
    currentEntry = 0
    found = False
    while (not found) and currentEntry <= maxEntry:
        t.GetEntry(currentEntry)
        found = (t.runnr == run)
        currentEntry += 1

    if not found:
        return OrderedDict()

    vars = get_dq_vars()
    values = OrderedDict()
    for var in vars:
        if not var[0] or hasattr(t, var[0] + '.score'):
            values[var[0]] = {
                    'name':  var[1],
                    'score': getattr(t, var[0] + '.score' if var[0] else 'score').value(),
                    'lvl':   getattr(t, var[0] + '.lvl'   if var[0] else 'lvl'  ).value()
                    }
    return values

class DqVarsParser(object):
    TELL1_NUMBERS_R   = range(42)
    TELL1_NUMBERS_PHI = range(64, 106)

    def __init__(self, trendingOnly = False, includeVars = False, expertMode = False):
        self.__trendingOnly = trendingOnly
        self.__includeVars = includeVars
        self.__expertMode = expertMode

    def get_dq_variables(self):
        return self.__add_variables_branch(Config().analysis_config, '', Config().analysis_config[0]['MasterCombiner'])
    
    def __add_variables_branch(self, config, key, branch, base = ''):
        result = []
    
        for child in branch['children']:
            if child in config[0].keys():
                result += self.__add_variables_branch(config, child, config[0][child], '{}.{}'.format(base, child) if base else child)
            elif child in config[1].keys():
                result += self.__add_variables_leaf(('{}.{}'.format(base, child)) if base else child, config[1][child])

        if result and self.__includeVars:
            result += [(key + '.' + var if key else var, branch['title']) for var in ['score']]

        return result

    def __add_variables_leaf(self, key, leaf):
        if self.__trendingOnly and not leaf.get('trending', False):
            return []

        result = []
        if self.__expertMode and "tell1" in leaf.keys():
            for tell1 in self.TELL1_NUMBERS_R + self.TELL1_NUMBERS_PHI:
                if self.__includeVars:
                    functionClass = getattr(veloview.analysis, leaf["function"])
                    vars = functionClass.vars()
                    result += [(key + '.' + key.split('.')[-1] + "_TELL1_{:0>3d}".format(tell1) + '.' + var, leaf['title'] + ' Tell1 {:0>3d}'.format(tell1)) for var in vars]
                else:
                    result.append((key + '.' + key.split('.')[-1] + "_TELL1_{:0>3d}".format(tell1), leaf['title'] + ' Tell1 {:0>3d}'.format(tell1)))

        if self.__includeVars:
            functionClass = getattr(veloview.analysis, leaf["function"])
            vars = functionClass.vars()
            return result + [(key + '.' + var, leaf['title']) for var in vars]
        return result + [(key, leaf['title'])]

