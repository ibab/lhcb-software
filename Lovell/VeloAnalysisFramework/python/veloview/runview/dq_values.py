"""Methods dealing with DQ values."""

import ROOT

from veloview.config import Config
from veloview.giantrootfile.gui_tree import Tree
from veloview.runview.utils import DqVarsParser

def get_dq_values(run):
    f = ROOT.TFile(Config().grf_file_path, 'READ')
    t = Tree(Config().grf_tree_name)
    t.GetEntry(1)
    vars = get_dq_vars()

    values = {}
    for var in vars:
        if not var[0] or hasattr(t, var[0] + '.score'):
            values[var[0]] = {
                    'name':  var[1],
                    'score': getattr(t, var[0] + '.score' if var[0] else 'score').value(),
                    'lvl':   getattr(t, var[0] + '.lvl'   if var[0] else 'lvl'  ).value()
                    }
    return values

def get_dq_vars():
    return DqVarsParser().get_dq_variables()

