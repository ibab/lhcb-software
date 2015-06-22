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
        parser.add_option("--json",
                          dest = "json",
                          default = False,
                          action = "store_true",
                          help = "JSON output format")
        parser.add_option("--readonly",
                          dest = "readonly",
                          default = False,
                          action = "store_true",
                          help = "Use the read only interface to Neo4j (for methods available)")
        

    def main(self):
        """ Main method for bootstrap and parsing the options.
        It invokes the appropriate method and  """
        self.log = logging.getLogger()
        opts = self.options
        args = self.args
        if opts.log_level == 'DEBUG':
            self.log.setLevel(logging.DEBUG)
        else:
            self.log.setLevel(logging.WARNING)

        if (len(args) == 0):
            self.log.error("Please specify a command name")
            self.parser.print_help()
            sys.exit(1)

        # Initializing the ConfDB interface
        if opts.readonly:
            from SoftConfDBRO import SoftConfDBRO
            self.mConfDB = SoftConfDBRO()
        else:
            self.mConfDB = SoftConfDB()
            
        if opts.log_level == 'DEBUG':
            self.mConfDB.log.setLevel(logging.DEBUG)
        else:
            self.mConfDB.log.setLevel(logging.WARNING)
            
        # Locating the command...
        cmdShort = { 'l':'listProjects', 'i':'listApplications', 'v':'listVersions', 'sp':'listStackPlatforms',
                    'p':'listPlatforms', 'd':'listDependencies', 'r':'listReferences',
                    'u':'listUsed', 'a':'listActive', 's':'listActiveReferences' }
        command = args[0]
        if command in cmdShort.keys():
            command = cmdShort[command]
        method =  self._findMethod(self, "cmd", command)

        if method == None:
            raise Exception("Could not find command '%s'. See --help for a list of commands" % command)

        # And invoking it...
        tmpargs = args[1:]
        # Setting to verbose mode
        if command.lower() == 'checkunused' and opts.verbose != None:
            tmpargs.append(opts.verbose)
        method(tmpargs)

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

    def cmdlistDatapkgs(self, args):
        ''' List the projects known by the SoftConfDB '''
        for p in sorted(self.mConfDB.listDatapkgs()):
            print p

    def cmdlistApplications(self, args):
        ''' List the projects known by the SoftConfDB '''
        for p in sorted(self.mConfDB.listApplications()):
            print p

    def cmdlistReleases(self, args):
        ''' List the projects known by the SoftConfDB '''
        for p in sorted(self.mConfDB.listReleaseReqs()):
            print "%s\t%s" % tuple(p)

    def cmdlistCMake(self, args):
        ''' List the projects built with CMake '''
        for p in sorted(self.mConfDB.listCMakeBuiltProjects()):
            print "%s\t%s" % tuple(p)

    def cmdlistTag(self, args):
        ''' List the projects with a given tag '''
        if self.options.json:
            import json
            print json.dumps(sorted(self.mConfDB.listTag(args[0].upper())), indent=2)
        else:
            for p in sorted(self.mConfDB.listTag(args[0].upper())):
                print "%s\t%s\t%s" % tuple(p)

    def cmdlistCMT(self, args):
        ''' List the projects built with CMT '''
        for p in sorted(self.mConfDB.listCMTBuiltProjects()):
            print "%s\t%s" % tuple(p)

    def cmdGetBuildTool(self, args):
        ''' List the projects built with CMT '''

        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]
        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        for p in self.mConfDB.getBuildTools(pname, pversion):
            print "%s" %  p

    def _sanitizeProjectName(sel, pname):
        ''' Puts back the correct case in display '''
        ret = pname
        try:
            from LbConfiguration.Project import getProject
            p = getProject(pname)
            ret = p.Name()
        except:
            # Ignore errors and return initial name
            pass
        return ret

    def cmdlistReleaseStacks(self, args):
        ''' List the projects known by the SoftConfDB '''
        stacks = self.mConfDB.listReleaseStacks()
        stackdicts = []
        for s in stacks:
            # We take the first one, it doesn't matter as this is a stack...
            (p, v) = next(iter(s))
            platforms = self.mConfDB.listStackPlatformsToRelease(p, v)
            stackdict = {}
            stackdict["projects"] = [ (self._sanitizeProjectName(p2), v2) for (p2, v2) in s ]
            stackdict["platforms"] = list(platforms)
            # Rather dodgy, we justtake the first one for the moment.
            # That doesn not sound right but only solution at short notice
            # We need to check the consistency of this in the future
            btools = self.mConfDB.getBuildTools(p, v)
            if len(btools) > 0:
                stackdict["build_tool"] = btools[0]
            stackdicts.append(stackdict)
                
        if self.options.json:
            import json
            print json.dumps(stackdicts, indent=2)
        else:
            for (i, sd) in enumerate(stackdicts):
                tmp =  ">>>>>>>>> Stack %d " %i
                if "build_tool" in sd:
                    tmp += " build_tool: %s " % sd["build_tool"]
                print "%s Platforms: %s" % (tmp, ",".join(sd["platforms"]))
                for (p, v) in sd["projects"]:
                    print "%d\t%s\t%s" % (i, p, v)

    def cmdlistActive(self, args):
        ''' List active projects '''
        for p in sorted(self.mConfDB.listActive()):
            print "%s %s" % (p[0], p[1])

    def cmdlistActiveApplications(self, args):
        ''' List active applications '''
        for p in sorted(self.mConfDB.listActiveApplications()):
            print "%s %s" % (p[0], p[1])

    def cmdlistUsed(self, args):
        ''' List used projects '''
        for p in sorted(self.mConfDB.listUsed()):
            print "%s %s" % (p[0], p[1])

    def cmdCheckUnused(self, args):
        ''' List used projects '''
        for p in sorted(self.mConfDB.checkUnused(args[0])):
            print "%s %s" % tuple(p)

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

    def cmdlistDatapkgVersions(self, args):
        ''' List the number of versions known for a given data package '''

        if (len(args) < 1):
            self.log.error("Please specify a data package name")
            sys.exit(1)

        allvs = self.mConfDB.listDatapkgVersions(args[0].upper())
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


    def cmdShow(self, args):
        ''' Check the various atributes of a specific node '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]

        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        self.mConfDB.show(pname, pversion)

        
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

    def cmdlistPlatformsToRelease(self, args):
        ''' List the Platforms released for a Couple project version '''
        if (len(args) < 2):
            self.log.error("Please specify a project and version")
            sys.exit(1)

        pname = args[0].upper()
        pversion =  args[1]
        if not self.cmdProjectExists(args):
            self.log.error("Could not find %s %s" % (pname, pversion))
            sys.exit(1)

        for p in sorted(self.mConfDB.listStackPlatformsToRelease(pname, pversion)):
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
            print "%s\t%s" %  tuple(p)

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
            print "%s\t%s" %  tuple(p)


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
            print "%s\t%s" %  tuple(p)


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

  %prog listProjects[l]                                    : List known projects
  %prog listActive[a]                                      : List active projects (i.e. installed on disk in AFS
  %prog listApplications[a]                                : List projects considered as applications
  %prog listUsed[u]                                        : List projects used by another project
  %prog listVersions[v] <project>                          : List known version of a given project
  %prog listStackPlatforms[sp] <project> <version>         : Show all projects the project depends on and teh known platforms
  %prog listPlatformsToRelease <project> <version>         : List the platforms this project should be released for
  %prog listPlatforms[p] <project> <version>               : List the platforms this project id known to be released for
  %prog listDependencies[d] <project> <version>            : List dependencies of a project/version
  %prog listReferences[r] <project> <version>              : List project/versions using this one
  %prog listActiveReferences[s] <project> <version>        : Like above only considering the ones that are active (on disk)
  %prog checkUnused <project> <version>                    : Check for unused projects/version
  %prog show <project> <version>                           : Show all properties and relationships of a project/version node
  %prog listReleases                                       : List projects flagged to be RELEASED
  %prog listReleaseStacks                                  : List projects flagged to be RELEASED grouping by stack with platforms
  %prog listCMake                                          : List projects built with CMake
  %prog listCMT                                            : List projects built with CMT
  %prog getBuildTool                                       : Get the build tool to use for this project
  %prog listDatapkgs                                       : List known Data packages
  %prog listDatapkgVersions <datapkg>                      : List known versions for the specified Data package
  %prog listTag <tag>                                      : List projects/version tagged with that specific value
      """
    s = LbSdbQuery(usage=sUsage)
    sys.exit(s.run())
