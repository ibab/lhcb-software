"""
Module with stripping selection line builder modules.
All line builders available via function lineBuilders().
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

import os

_strippingModules = {}
WGs = []
wgDirs = os.listdir(os.environ['STRIPPINGARCHIVEROOT']+'/python/StrippingArchive')
for dirs in [ dir for dir in wgDirs if 'Stripping' in dir ]:
  WGs.append(dirs[9:])

for WG in WGs:
  _tmpModule = __import__('StrippingArchive.Stripping'+WG,fromlist=['_strippingModules'])
  _strippingModules[WG] = _tmpModule._strippingModules

from StrippingUtils.Utils import getLineBuildersFromModule as _getter
from StrippingUtils.Utils import getBuilderConfFromModule as _getcon

def lineBuilders(WGs = None) :
    """
    Return all the line builders in the module for a
    requested list of WG.
    """
    _lineBuilders = {}
    if not WGs:
      for wg in _strippingModules.keys():
        for _sm in _strippingModules[wg]:
          _lineBuilders.update(_getter(_sm))
    elif any( WG for WG in _strippingModules.keys() if WG in WGs ):
      for wg in WGs:
        for _sm in _strippingModules[wg]:
          _lineBuilders.update(_getter(_sm))
    else:
      raise Exception( 'The requested WG does not exists' )

    return dict(_lineBuilders)


def buildersConf(WGs = None) :
    """
    Return all the line builders in the module for a
    requested list of WG.
    """
    _buildersConf = {}
    if not WGs:
      for wg in _strippingModules.keys():
        for _sm in _strippingModules[wg]:
          duplicates=list(set(_buildersConf.keys()) & set(_getcon(_sm).keys()) )
          if not duplicates:
            _buildersConf.update(_getcon(_sm))
          else:
            print "The following names have already been used:"
            print ', '.join(duplicates)
            print "Please change the name of your configuration"
            sys.exit(1)
    elif any( WG for WG in _strippingModules.keys() if WG in WGs ):
      for wg in WGs:
        for _sm in _strippingModules[wg]:
          duplicates=list(set(_buildersConf.keys()) & set(_getcon(_sm).keys()) )
          if not duplicates:
            _buildersConf.update(_getcon(_sm))
          else:
            print "The following names have already been used:"
            print ', '.join(duplicates)
            print "Please change the name of your configuration"
            sys.exit(1)
    else:
      raise Exception( 'The requested WG does not exists' )

    return dict(_buildersConf)

