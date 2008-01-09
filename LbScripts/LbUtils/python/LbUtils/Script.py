from LbUtils.Option import Parser
from LbUtils import Env
import logging

import sys

class Script():
    _version = "$Name: not supported by cvs2svn $"
    def __init__(self, usage=None, version = None):
        if version is None:
            version = self._version 
        self.parser = Parser(usage=usage, version=version)
        self.env = Env.getDefaultEnv()
        self.log = logging.getLogger()
        self.options = None
        self.args = None
    def defineOpts(self):
        # User options
        # self.parser.add_options(...)
        pass
    def main(self):
        # User code
        pass
    def parseOpts(self,args):
        self.options, self.args = self.parser.parse_args(args)
    def run(self, args=sys.argv[1:]):
        self.defineOpts()
        self.parseOpts(args)
        self.main()
        