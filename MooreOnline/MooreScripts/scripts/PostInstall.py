#!/usr/bin/env python
import sys

#sys.argv[1] must be the location of the setup shell file
if len(sys.argv)<2:
    raise AttributeError("Not enough inputs...")

setup=sys.argv[1]

def check_path():
    """
    Raise a damn exception if you have called this from the wrong place!!
    """
    import os
    import re
    cwd=os.path.abspath('.').lower()
    updir=os.path.abspath('..').lower()
    lastdir=cwd.split('/')[-1]
    if (not re.match("mooreonline_((v[0-9]*r.*)|(head))",lastdir)) or updir!=os.path.abspath(os.environ["User_release_area"]).lower():
        raise IOError("Hey! you're calling this script from the wrong directory. You'd better call it from a valid Moore directory in a User_release_area! "+cwd+" vs."+os.path.abspath(os.environ["User_release_area"]))

def define_cmtconfig() :
    import os
    return os.environ['CMTCONFIG']

def define_prefix(cmtconfig) :
    import os
    return os.path.abspath('InstallArea/%s'%(cmtconfig))

check_path()
cmtconfig=define_cmtconfig()
prefix = define_prefix(cmtconfig)
from MooreScripts.CreateSetup import StripSetupMoore
StripSetupMoore(setup)
from MooreScripts.CreateRunEFF import CreateRunMooreOnline_EFF

for split in [ '','Hlt1','Hlt2'] : 
    CreateRunMooreOnline_EFF( "%s/runMoore%sOnline_EFF.sh"%(prefix,split), setup,split )


