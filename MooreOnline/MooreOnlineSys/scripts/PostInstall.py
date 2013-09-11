#!/usr/bin/env python
def define_current_version() :
    import fileinput
    for line in fileinput.input('MooreOnlineSys/cmt/version.cmt'): version =  line.strip()
    return version

def define_release_area() :
    import os
    os.environ['User_release_area']= os.path.abspath('..')

def define_prefix() :
    import os
    return os.path.abspath('InstallArea/scripts/')

version = define_current_version()
define_release_area()
import os
prefix = define_prefix()
from CreateSetupMoore import CreateSetupMoore
setup = '%s/setupMoore_%s.sh'% (prefix,version)
CreateSetupMoore(setup, version)

from CreateRunMooreOnline_EFF import CreateRunMooreOnline_EFF
for split in [ '','Hlt1','Hlt2'] : 
    CreateRunMooreOnline_EFF( "%s/runMoore%sOnline_EFF.sh"%(prefix,split), setup,split )


