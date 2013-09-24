#!/usr/bin/env python
import sys
spOps=None
if len(sys.argv)>1:
    if ";" in sys.argv[1] or ":" in sys.argv[1] or "\n" in sys.argv[1] or "\\" in sys.argv[1]:
        raise ValueError("Nice try, but I'm not letting you inject obvious python code into my program. Make it less obvious, and I'll consider it.")
    if not sys.argv[1].lstrip().startswith("[") or not sys.argv[1].rstrip().endswith("]"):
        raise ValueError("The first option needs to be a list to send to SetupProject, it must start and end with square brackets")
    exec("spOps="+sys.argv[1])
    print "using extra setupProject options ..."+str(spOps)

def check_path():
    """
    Raise a damn exception if you have called this from the wrong place!!
    """
    import os
    import re
    cwd=os.path.abspath('.').lower()
    lastdir=cwd.split('/')[-1]
    if not re.match("mooreonline_((v[0-9]*r.*)|(head))",lastdir):
        raise IOError("Hey! you're calling this script from the wrong directory. You'd better call it from a valid User_release_area! "+cwd)

def define_current_version() :
    """
    Get Moore version from cmt, for now...
    """
    import subprocess
    process=subprocess.Popen(['cmt','sh','projects'],stdout=subprocess.PIPE)
    out,err=process.communicate()
    for line in out.split('\n'):
        if "MOOREONLINE_" in line:
            version=line.split("_")[1].split(" ")[0].strip()
            return version

def define_release_area() :
    """
    Set the user release area. So that it is definately included in the environment even if you didn't do SetupProject yet..?
    This is not really so safe
    """
    import os
    os.environ['User_release_area']= os.path.abspath('..')

def define_cmtconfig() :
    import os
    return os.environ['CMTCONFIG']

def define_prefix(cmtconfig) :
    import os
    return os.path.abspath('InstallArea/%s/scripts/'%(cmtconfig))

check_path()
version = define_current_version()
define_release_area()
cmtconfig=define_cmtconfig()
import os
prefix = define_prefix(cmtconfig)
from MooreScripts.CreateSetup import CreateSetupMoore
setup = '%s/setupMoore_%s.sh'% (prefix,version)
CreateSetupMoore(setup, version, spOps)

from MooreScripts.CreateRunEFF import CreateRunMooreOnline_EFF
for split in [ '','Hlt1','Hlt2'] : 
    CreateRunMooreOnline_EFF( "%s/runMoore%sOnline_EFF.sh"%(prefix,split), setup,split )


