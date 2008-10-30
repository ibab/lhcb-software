# $Id: Script.py,v 1.8 2008-10-30 09:11:10 hmdegaud Exp $
from LbUtils.Option import Parser
from LbUtils import Env
import logging

import sys

class Script:
    _version = "$Id: Script.py,v 1.8 2008-10-30 09:11:10 hmdegaud Exp $".replace("$","").replace("Id:","").strip()
    _description = ""
    def __init__(self, usage=None, version = None, parser=Parser, help_output=sys.stdout, description=None):
        """ constructor of the Script. provides default options parser 
        and logger """
        if version is None:
            version = self._version 
        if description is None :
            description = self._description
        self.parser = parser(usage=usage, version=version, help_output=help_output, description=description)
        self.env = Env.getDefaultEnv()
        self.log = logging.getLogger()
        self.options = None
        self.args = None
    def defineOpts(self):
        """ User options -- has to be overridden """
        # self.parser.add_options(...)
        pass
    def main(self):
        """ User code place holder -- has to be overridden """
        return 0
    def parseOpts(self, args):
        """ do the parsing (behind the scene) """
        self.options, self.args = self.parser.parse_args(args)
    def run(self, args=sys.argv[1:]):
        """ main function to be called by the user """
        self.defineOpts()
        self.parseOpts(args)
        return self.main()
        