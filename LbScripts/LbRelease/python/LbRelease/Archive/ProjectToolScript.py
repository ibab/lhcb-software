'''
Ancestor to tools that deal with projects.

It automatically parses the arguments to find the project and version.
It also sets the logging and has a utility method to check grid credentials

Created on Mar 22, 2012
@author: Ben Couturier
'''

import logging
import sys
from LbUtils.Option import Parser
from LbUtils.Script import Script

#
# Exceptions thrown by the methods
###############################################################################
class DiracNotFoundException(Exception):
    """ Exception class to indicate that SetupProject LHCbDirac
        was probably not done """
    def __init__(self, message):
        super(DiracNotFoundException, self).__init__(message)

class InvalidGridCredentialsException(Exception):
    """ Exception class to indicate that the grid credentials
        are missing """
    def __init__(self, message = None):
        super(InvalidGridCredentialsException, self).__init__(message)

#
# Main script class
###############################################################################
class ProjectTool(Script):
    """ Ancestor for tools dealing with projects """
    def __init__(self, usage=None, version=None, parser=Parser, description=None):
        """ Contructor with default values for scripts"""
        Script.__init__(self, usage=usage, version=version, parser=parser, description=description)
        self.projectName = None
        self.version = None
        self.addDebugOpts()

    def addDebugOpts(self):
        parser = self.parser
        parser.add_option("-d", action="store_true",
                          dest="debug",
                          help="Enable debug mode")
        parser.set_defaults(debug=False)

    def setLogger(self):
        """ Setup the logging parameters """
        self.log = logging.getLogger()
        for h in self.log.handlers:
            self.log.removeHandler(h)

        console = logging.StreamHandler()
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        console.setFormatter(formatter)
        if self.options.debug == 1 :
            #console.setFormatter(logging.Formatter("%(levelname)-8s: %(funcName)-25s - %(message)s"))
            self.log.setLevel(logging.DEBUG)
        else :
            #console.setFormatter(logging.Formatter("%(levelname)-8s: %(message)s"))
            self.log.setLevel(logging.INFO)
        self.log.addHandler(console)

    def checkCredentials(self, requested_group):
        """ Utility method to check the credentials and that the use belongs
        to a specific group """
        # First locate the DIRAC code
        try:
            from DIRAC.Core.Security.ProxyInfo import getProxyInfo
        except:
            raise DiracNotFoundException("DIRAC not found")
        # Now check the credentials themselves

        # Now using dirac to lookup the proxy info
        try:
            info = getProxyInfo()
        except Exception,e:
            raise InvalidGridCredentialsException(str(e))

        if not info['OK']:
            raise InvalidGridCredentialsException(str(info))

        # Checking the validity
        seconds_left = info['Value']['secondsLeft']
        if seconds_left == 0:
            raise InvalidGridCredentialsException("proxy expired")

        # Checking the group
        effective_group = info['Value']['group']
        if (effective_group != requested_group):
            raise InvalidGridCredentialsException("incorrect group: %s instead of %s" % (effective_group, requested_group))

    def main(self):
        """ Common main for all the scripts """
        # Setup and check the arguments
        args = self.args

        if len(args) < 2:
            print self.parser.get_usage()
            sys.exit(1)

        self.setLogger()
        self.projectName = args[0]
        self.version = args[1]
        self.projectToolMain(self.projectName, self.version)

    def projectToolMain(self, projectName, version):
        """ Asbtract method that subclasses have to implement """
        raise NotImplementedError()