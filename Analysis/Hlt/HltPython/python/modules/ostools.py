# =============================================================================
""" @namespace ostools
@brief extra functions related with the operative system
@author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
@date 2009-08-13
"""
# =============================================================================

import os
import time as _time

def version(name="DAVINCI"):
    """ returns the version of a package
    @param package name, i.e DaVinci, HltSys
    """
    name = name.upper() +"ROOT"
    path = os.environ[name]
    if (not path): return None
    i = path.rfind("/")
    version = path[i+1:]
    return version

def time():
    """ returns a string with the month+data+hour+minute i.e 08121200
    """
    ts = _time.localtime()
    format = "%02d"
    s = ""
    for i in range(1,5): s = s+str((format%ts[i]))
    return s

def command(com):
    """ execute a string as a command. i.e (ls)
    """
    print com
    os.system(com)
