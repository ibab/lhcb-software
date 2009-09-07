#!/usr/bin/env python
"""
main script for the LHCb Nightlies launching.

Can be launched in one of the following ways:
1) startNightlies.py run slotName [j_value [platform1 [platform2 [...]]]]
   Starts Nightlies procedure for <slotName>, optionally with compiler -j
   value set followed by the list of platforms building process in this
   run will be limited.
2) startNightlies.py runparallel slotName [j_value]
   Starts Nightlies procedure for <slotName>, compiling all platforms in
   slot in parallel. Optionally -j value can be given to the compiler.
3) startNightlies.py clean slotName
   Cleans local build directory for the slot. Has to be done before
   Nightlies are launched.
4) startNightlies.py cleanAFS --delete-from-today slotName
   Cleans AFS directory for the slot.
"""

import sys
import os
from os import linesep
from LbRelease.Nightlies.setup import configurationPath
if not os.environ.has_key('LCG_XMLCONFIGDIR'): os.environ['LCG_XMLCONFIGDIR'] = configurationPath
from LbRelease.Nightlies.functions import config, get, install, test, clean, cleanAFSSpace, run, runparallel
from LbUtils.Script import Script

def usage():
    ret  = 'usage: startNightlies run slotName j_value l_value [platform1 [platform2 [...]]]'
    ret += linesep + '       clean slotName'
    ret += linesep + '       cleanAFS slotName (be careful!)'
    ret += linesep + __doc__
    return ret

class StartNightliesScript(Script):
    def main(self):
        if len(self.args) <2:
            print usage()
            raise SystemExit
        function = {
                     'config' : config,
                     'get' : get,
                     'install' : install,
                     'test' : test,
                    }
        if self.args[0] == 'clean':
            clean(self.args[1])
        elif self.args[0] == 'cleanAFS':
            cleanAFSSpace(self.args[1])
        elif self.args[0] == 'run' and len(self.args) >= 5:
                platss = " ".join([self.args[x] for x in range(4,len(self.args))])
                run(slotName=self.args[1], minusj=self.args[2], minusl=self.args[3], platforms=platss)
        elif self.args[0] == 'run' and len(self.args) == 4:
            run(slotName=self.args[1], minusj=self.args[2], minusl=self.args[3])
        else:
            print usage()
            raise SystemExit

if __name__ == '__main__':
    s = StartNightliesScript(usage=usage(), description="")
    s.run()
