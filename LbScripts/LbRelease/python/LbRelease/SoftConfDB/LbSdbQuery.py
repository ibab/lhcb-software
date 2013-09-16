'''
Created on May 6, 2013

@author: Ben Couturier
'''
from SoftConfDB import SoftConfDB
from LbUtils.Script import Script
from LbConfiguration.Version import sortVersions

import collections
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
        cmdShort = { 'l':'listProjects', 'v':'listVersions', 'sp':'listStackPlatforms',
                    'p':'listPlatforms', 'd':'listDependencies', 'r':'listReferences',
                    'u':'listUsed', 'a':'listActive', 's':'listActiveReferences' }
        command = args[0]
        if command in cmdShort.keys():
            command = cmdShort[command]
        method =  self._findMethod(self, "cmd", command)

        # And invoking it...
        method(args[1:])

    # Know commands forwarded to the DB
    ###########################################################################
    def cmdProjectExists(self, args):
        ''' List the projects known by the SoftConfDB '''
        p = self.mConfDB.findVersion(args[0].upper(), args[1])
        return (len(p) > 0)

    def cmdlistProjects(self, args):
        ''' List the projects known by the SoftConfDB '''
        for p in sorted(self.mConfDB.listProjects()):
            print p

    def cmdlistActive(self, args):
        ''' List active projects '''
        for p in sorted(self.mConfDB.listActive()):
            print "%s %s" % (p[0], p[1])

    def cmdlistUsed(self, args):
        ''' List used projects '''
        for p in sorted(self.mConfDB.listUsed()):
            print "%s %s" % (p[0], p[1])

    def cmdCheckUnused(self, args):
        ''' List used projects '''
        for p in sorted(self.mConfDB.checkUnused()):
            print "%s %s" % p

    def cmdlistVersions(self, args):
        ''' List the number of versions known for a given project '''

        if (len(args) < 1):
            self.log.error("Please specify a project name")
            sys.exit(1)

        allvs = self.mConfDB.listVersions(args[0].upper())
        if len(allvs) > 0:
            proj =  allvs[0][0]
            if allvs[0][1].startswith("v"):
                # Checking if we have LHCb Ordering
                vs = sortVersions([ t[1] for t in allvs ], safe=True)
            else:
                # Or the normal sorting order
                vs = sorted([ t[1] for t in allvs ])

            # Now print out the results
            for v in vs:
                print "%s %s" %  (proj, v)

    def cmdlistStackPlatforms(self, args):
        ''' The project versions available in the stack '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]
        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        projPlatforms = collections.defaultdict(list)
        for p in sorted(self.mConfDB.listStackPlatforms(pname, pversion), key = lambda ret: ret[1]):
            key = p[1] + "_" + p[2]
            projPlatforms[key].append(p[0])

        for k in sorted(projPlatforms.keys()):
            print "%s :\t%s" % (k.replace("_", " "), ", ".join(sorted(projPlatforms[k])))

    def cmdlistPlatforms(self, args):
        ''' List the Platforms released for a Couple project version '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]
        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        for p in sorted(self.mConfDB.listPlatforms(pname, pversion)):
            print p


    def cmdlistDependencies(self, args):
        ''' List the project/versions the specified project depends on '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]
        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        for p in self.mConfDB.listDependencies(pname, pversion):
            print "%s\t%s" %  p

    def cmdlistReferences(self, args):
        ''' List the project/versions that depend on this project '''

        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]
        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        for p in self.mConfDB.listReferences(pname, pversion):
            print "%s\t%s" %  p


    def cmdlistActiveReferences(self, args):
        ''' List the project/versions that depend on this project '''

        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]
        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        for p in self.mConfDB.listActiveReferences(pname, pversion):
            print "%s\t%s" %  p


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

  %prog listProjects[l]
  %prog listActive[a]
  %prog listUsed[u]
  %prog listVersions[v] <project>
  %prog listStackPlatforms[sp] <project> <version>
  %prog listPlatforms[p] <project> <version>
  %prog listDependencies[d] <project> <version>
  %prog listReferences[r] <project> <version>
  %prog listActiveReferences[s] <project> <version>

      """
    s = LbSdbQuery(usage=sUsage)
    sys.exit(s.run())
