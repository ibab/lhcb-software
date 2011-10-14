#!/usr/bin/env python
""" install_project.py post install script for the LbScripts project """

import sys
import os

# bootstraping the location of the file
_this_file = os.path.abspath(sys.argv[0])
_cmt_dir = os.path.dirname(_this_file)
_sys_dir = os.path.dirname(_cmt_dir)
_prj_dir = os.path.dirname(_sys_dir)
_ia_dir = os.path.join(_prj_dir, "InstallArea" )
_iapy_dir = os.path.join(_ia_dir, "python")

if os.path.isdir(_iapy_dir) :
    sys.path.insert(0, _iapy_dir)

from LbUtils.Script import Script

__version__ = "$Id$"

class PostInstallScript(Script):
    _version = __version__
    _description = __doc__
    def defineOpts(self):
        parser = self.parser

    def main(self):
        opts = self.options
        args = self.args
        genlogscript = os.path.join(_ia_dir, "scripts", "generateLogin")
        gencmd = [ genlogscript ]
        if opts.log_level == "DEBUG" :
            gencmd.append("--debug")
        print gencmd
        return 0

if __name__ == '__main__':
    s = PostInstallScript(usage="%prog [options]")
    sys.exit(s.run())

