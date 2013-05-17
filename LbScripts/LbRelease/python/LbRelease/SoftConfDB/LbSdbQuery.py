'''
Created on May 6, 2013

@author: Ben Couturier
'''
from SoftConfDB import SoftConfDB
from LbUtils.Script import Script

import logging
import sys
import inspect

class LbSdbQuery(Script):
    """ Main scripts class for looking up dependencies.
    It inherits from """

    def defineOpts(self):
        """ Script specific options """
        parser = self.parser
        parser.add_option("-d",
                          dest = "debug",
                          action = "store_true",
                          help = "Display debug output")

    def main(self):
        """ Main method for bootstrap and parsing the options.
        It invokes the appropriate method and  """
        self.log = logging.getLogger()
        opts = self.options
        args = self.args
        if opts.debug:
            self.log.setLevel(logging.DEBUG)
        else:
            self.log.setLevel(logging.WARNING)

        if (len(args) == 0):
            self.log.error("Please specify a command name")
            self.parser.print_help()
            sys.exit(1)

        # Initializing the ConfDB interface
        self.mConfDB = SoftConfDB()

        # Locating the command...
        command = args[0]
        method =  self._findMethod(self, "cmd", command)

        # And invoking it...
        method(args[1:])

    # Know commands forwarded to the DB
    ###########################################################################
    def cmdlistProjects(self, args):
        ''' List the projects known by the SoftConfDB '''
        for p in self.mConfDB.listProjects():
            print p

    def cmdlistVersions(self, args):
        ''' List the number of versions known for a given project '''

        if (len(args) < 1):
            self.log.error("Please specify a project name")
            sys.exit(1)

        for p in self.mConfDB.listVersions(args[0].upper()):
            print "%s\t%s" %  p

    def cmdlistStackPlatforms(self, args):
        ''' The project versions available in the stack '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        for p in self.mConfDB.listStackPlatforms(args[0].upper(), args[1]):
            print p

    def cmdlistPlatforms(self, args):
        ''' List the Platforms released for a Couple project version '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        for p in self.mConfDB.listStackPlatforms(args[0].upper(), args[1]):
            print p


    def cmdlistDependencies(self, args):
        ''' List the project/versions the specified project depends on '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        for p in self.mConfDB.listStackPlatforms(args[0].upper(), args[1]):
            print p

    def cmdlistReferences(self, args):
        ''' List the project/versions that depend on this project '''

        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        for p in self.mConfDB.listStackPlatforms(args[0].upper(), args[1]):
            print p



    # Method that looks for a method name in the object passed
    # and returns it in order to set the attribute
    def _findMethod(self, obj, prefix, partialMethodName):
        """ Find the method named prefix + partialMethodName in the object instance """
        allmethods = inspect.getmembers(obj, predicate=inspect.ismethod)
        foundMethod = None
        for m in allmethods:
                if m[0].lower() == (prefix + partialMethodName.lower()):
                    self.log.debug("## Found Method " + m[0])
                    foundMethod = m[1]
        return foundMethod


if __name__=='__main__':
    sUsage = \
    """%prog Command to query the Software Configuration database, which can be invoked in the following way:

  %prog listProjects
  %prog listVersions <project>
  %prog listStackPlatforms <project> <version>
  %prog listPlatforms <project> <version>
  %prog listDependencies <project> <version>
  %prog listReferences <project> <version>


      """
    s = LbSdbQuery(usage=sUsage)
    sys.exit(s.run())




if __name__ == '__main__':
    pass
