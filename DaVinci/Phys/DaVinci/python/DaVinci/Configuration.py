"""
High level configuration tools for DaVinci
At the moment this doesn't do anything. Waiting for re-structuring of
configuratables.
"""
__version__ = "$Id: Configuration.py,v 1.1 2008-11-04 10:24:41 jpalac Exp $"
__author__ == "Juan Palacios <juan.palacios@nikhef.nl>"

from LHCbKernel.Configuration import *
from GaudiConf.Configuration import *

class DaVinci(LHCbConfigurableUser) :
    __slots__ = {
        "EvtMax"     :  -1, # Maximum number of events to process
        "skipEvents" :   0,     # events to skip
        "mainOpts"   : '$DAVINCIROOT/options/DaVinci.py',
        "DDDBtag"    : 'DC06-default', #
        "CondDBtag"  : 'DC06-default'
        }
