"""Methods dealing with DQ values."""

from collections import OrderedDict
import ROOT

import veloview.analysis
from veloview.config import Config
from veloview.core.io import DQDB

def get_comment(runnr):
    db = DQDB(Config().dq_db_file_path)
    value = db.get_comment(runnr)
    db.close()
    return value

def set_comment(runnr, comment):
    db = DQDB(Config().dq_db_file_path)
    db.set_comment(runnr, comment)
    db.close()

def get_trending_variables(expert = False):
    dqVarsParser = DqVarsParser(True, True, expert)
    return dqVarsParser.get_dq_variables()

def get_dq_vars():
    return DqVarsParser().get_dq_variables()

def get_dq_values(runnr):
    db = DQDB(Config().dq_db_file_path)
    values = db.read(runnr)
    db.close()
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
            result += [(key + '.' + var if key else var, branch['title'] + ' DQ score') for var in ['score']]

        return result

    def __add_variables_leaf(self, key, leaf):
        if self.__trendingOnly and not leaf.get('trending', False):
            return []

        sensor_dep = leaf.get("sensor_dependent", False)

        result = []
        if self.__expertMode and sensor_dep:
            for tell1 in self.TELL1_NUMBERS_R + self.TELL1_NUMBERS_PHI:
                if self.__includeVars:
                    functionClass = getattr(veloview.analysis, leaf["comparison"])
                    vars = functionClass.vars()
                    result += [(key + '.' + key.split('.')[-1] + "_TELL1_{:0>3d}".format(tell1) + '.' + var, leaf['title'] + ' Tell1 {:0>3d}'.format(tell1)) for var in vars]
                else:
                    result.append((key + '.' + key.split('.')[-1] + "_TELL1_{:0>3d}".format(tell1), leaf['title'] + ' Tell1 {:0>3d}'.format(tell1)))

        if self.__includeVars and not sensor_dep:
            vars = []
            for trend in leaf.get("trending", []):
                trendClassName = trend[0] if isinstance(trend, tuple) else trend
                trendClass = getattr(veloview.analysis, trendClassName)
                vars += trendClass.vars()
            return result + [(key + '.' + var, leaf['title'] + ' ' + var) for var in vars]

        return result + [(key, leaf['title'] + ' DQ score')]

