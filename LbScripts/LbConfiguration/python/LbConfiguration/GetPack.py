#!/usr/bin/env python
# $Id: GetPack.py,v 1.4 2008-11-28 10:45:22 marcocle Exp $

from LbUtils.Script import Script
from LbUtils import rcs
import os, sys, re
from subprocess import Popen, PIPE, STDOUT

## Small class for the generic manipulation of RevisionControlSystem URLs
class RepositoryInfo(object):
    __protocols__ = []
    def __init__(self, protocol, host, path, user = None):
        self.protocol = protocol
        self.host = host
        self.path = path
        self.user = user
    
    def _get_protocol(self):
        return self._protocol
    def _set_protocol(self, protocol):
        if protocol not in self.__protocols__:
            raise RuntimeError("Invalid protocol '%s' (allowed: %r)" % (protocol, self.__protocols__))
        self._protocol = protocol
    ## Property for the validation of the protocol
    protocol = property(_get_protocol, _set_protocol)
    
    def _get_path(self):
        return self._path
    def _set_path(self, path):
        if not path.startswith("/"):
            self._path = "/" + path
        else:
            self._path = path
    ## Property for the validation of the path
    path = property(_get_path, _set_path)

## Subversion specific implementation of RepositoryInfo
class SVNReposInfo(RepositoryInfo):
    __protocols__ = ["svn", "svn+ssh", "file", "https"]
    def __str__(self):
        # prepare url template
        if self.user:
            url = "%(_protocol)s://%(user)s@%(host)s%(_path)s"
        else:
            url = "%(_protocol)s://%(host)s%(_path)s"
        return url % self.__dict__

## CVS specific implementation of RepositoryInfo
class CVSReposInfo(RepositoryInfo):
    __protocols__ = ["kserver", "ext", "pserver"]
    def __str__(self):
        if self._protocol == "ext" and 'CVS_RSH' not in os.environ:
            raise RuntimeError("Cannot use 'ext' protocol without the environment variable 'CVS_RSH' set (e.g. ssh or plink)")
        # prepare url template
        if self.user:
            url = ":%(_protocol)s:%(user)s@%(host)s:%(_path)s"
        else:
            url = ":%(_protocol)s:%(host)s:%(_path)s"
        return url % self.__dict__

## List of known repositories
#  @todo: temporarily here, but should be moved.
__repositories__ = { "gaudi": { "ssh":       SVNReposInfo("svn+ssh", "svn.cern.ch", "/reps/gaudi"),
                                "anonymous": SVNReposInfo("https", "svnweb.cern.ch", "/guest/gaudi") },
                     "lhcb":  { "kserver":   CVSReposInfo("kserver", "isscvs.cern.ch", "/local/reps/lhcb"),
                                "ssh":       CVSReposInfo("ext", "isscvs.cern.ch", "/local/reps/lhcb"),
                                "anonymous": CVSReposInfo("pserver", "isscvs.cern.ch", "/local/reps/lhcb", "anonymous") },
                     }
# Define default repositories
__repositories__["gaudi"]["default"] = __repositories__["gaudi"]["ssh"]
if sys.platform.startswith("linux"):
     __repositories__["lhcb"]["default"] = __repositories__["lhcb"]["kserver"]

## Class used as exception to allow a "quit" in any moment
class Quit:
    pass

## @class GetPack
# Main script class for getpack.
class GetPack(Script):
    _version = "$Id: GetPack.py,v 1.4 2008-11-28 10:45:22 marcocle Exp $".replace("$","").replace("Id:","").strip()
    def __init__(self):
        Script.__init__(self, usage = "\n\t%prog [options] package [ [version] ['tag'|'head'] ]"
                                      "\n\t%prog [options] -i [repository [hat]]"
                                      "\n\t%prog [options] --project project version",
                              description = "script to checkout/update and cmt-configure packages"
                                            " from LHCb CVS or Subversion repositories")
        self._packages = None
        self._projects = None
        self._repositories = None
        
        self.selected_repository = None
        self.selected_hat = None
        
        self.requested_package = None
        self.requested_package_version = None

        self.show_uses_regexp = re.compile(r"# (?P<spaces> *)use (?P<package>[^ ]+) (?P<version>[^ ]+) (?P<hat>[^ ]*)")
        
    def defineOpts(self):
        """ User options -- has to be overridden """
        self.parser.add_option("-i", "--interactive", action = "store_true",
                               help = "prompt for the package to get (using repository/hat to refine the list)")
        self.parser.add_option("-r", "--recursive", action = "store_true",
                               help = "checkout all packages this one directly depends on")
        self.parser.add_option("-H", "--recursive-head", action = "store_true",
                               help = "check out all subsequent packages with head revision")
        self.parser.add_option("-R", "--really-recursive", action = "store_true",
                               help = "check out >every< package this one depends on")
        self.parser.add_option("-p", "--protocol",
                               help = "preferred access method to revision system [kserver|ssh|anonymous]"
                                      " (kserver is valid only for CVS)")
        self.parser.add_option("-f",
                               dest = "protocol",
                               help = "obsolete: same as -p, --protocol")
        self.parser.add_option("-u", "--unversioned", action = "store_false",
                               dest = "version_dirs",
                               help = "do not use versioned subdirectories for the packages [default]")
        self.parser.add_option("-V", "--versioned", action = "store_true",
                               dest = "version_dirs",
                               help = "use versioned subdirectories for the packages")
        self.parser.add_option("--user",
                               help = "username to use to connect to the repositories. "
                                      "By default, the user is not explicitely used "
                                      "unless a user name is specified with the "
                                      "environment variable GETPACK_USER.")
        self.parser.add_option("-P", "--project", action = "store_true",
                               help = "create a project top level directory")
        self.parser.set_defaults(protocol = "default",
                                 version_dirs = False)
        if "GETPACK_USER" in os.environ:
            self.parser.set_defaults(user = os.environ["GETPACK_USER"])
    
    def _listChoice(self, message, lst):
        page_size = 30
        count = len(lst)
        page = 0
        pages = count/page_size + 1
        
        default_ans = None
        idx = -1
        while idx < 0:
            # display current page
            print message
            for ln in range(page*page_size, min((page+1)*page_size, count)):
                print "%3d - %s" % (ln, lst[ln])
            # prepare the question (with default answer)
            question = "Please enter your choice (0-%d, " % (count-1)
            if page < pages - 1:
                question += "(n)ext, "
            if page > 0:
                question += "(p)revious, "
            if not default_ans:
                default_ans = 'q'
                if page < pages - 1:
                    default_ans = 'n'
                elif page > 0:
                    default_ans = 'p'
            question += "(q)uit) [%s]: " % default_ans
            
            ans = None
            while ans is None:
                sys.stdout.write(question)
                ans = sys.stdin.readline().strip().lower()
                if ans == "":
                    ans = default_ans 
                if ans in ['n', 'next']:
                    page += 1
                    default_ans = ans
                    if page >= pages:
                        page = pages - 1
                    if page == pages - 1:
                        default_ans = 'p'
                elif ans in ['p', 'prev', 'previuos']:
                    page -= 1
                    default_ans = ans
                    if page < 0:
                        page = 0
                    if page == 0:
                        default_ans = 'n'
                elif ans in [ 'q', 'quit' ]:
                    raise Quit
                else:
                    try:
                        ans = int(ans)
                    except:
                        ans = -1
                    if ans < 0 or ans >= count:
                        print "Invalid selection '%s'" % ans
                        ans = None
                    else:
                        idx = ans 
        return idx
    
    def _askVersion(self, versions):
        ans = None
        default = versions[0]
        while not ans:
            sys.stdout.write("Select a version (%s, (h)ead, (q)uit [%s]): "%
                             (", ".join(versions),default))
            ans = sys.stdin.readline().strip()
            if ans == '':
                ans = default
            elif ans.lower() in [ 'h', 'head' ]:
                return "head"
            elif ans.lower() in [ 'q', 'quit' ]:
                raise Quit
            elif ans not in versions:
                print "Version '%s' not valid!" % ans
                ans = None
        return ans
    
    def _doCMTConfig(self, cwd):
        if os.path.exists(os.path.join(cwd,"requirements")):
            self.log.info("Executing 'cmt config' in '%s'", cwd)
            proc = Popen(["cmt", "config"], cwd = cwd, stdout = PIPE, stderr = PIPE)
            out, err = proc.communicate()
            ## @todo: check for success of 'cmt config'
            if sys.platform.startswith("win"):
                self.log.info("Executing 'cmt build vsnet' in '%s'", cwd)
                proc = Popen(["cmt", "build", "vsnet"], cwd = cwd, stdout = PIPE, stderr = PIPE)
                out, err = proc.communicate()
                ## @todo: check for success of 'cmt build vsnet'
        else:
            self.log.warning("Cannot find requirements file, 'cmt config' skipped.")
            
    def checkout(self, package, version = "head"):
        reps = self.packages[package]
        if len(reps) > 1:
            lst = []
            for k in reps:
                lst.append('%s (%s)' % (k, self.repositories[k]))
            idx = self._listChoice("Package '%s' in more than one repository, choose the repository." % package,
                                   lst)
        else:
            idx = 0
        rep = self.repositories[reps[idx]]
        if version != "head": # head is always valid
            versions = rep.listVersions(package)
            if not versions:
                raise RuntimeError("No version found for package '%s'" % package)
            if (not version) or (version not in versions):
                if version:
                    self.log.warning("Version '%s' not found for package '%s'" % (version, package))
                version = self._askVersion(versions)
        self.log.info("Checking out %s %s (from '%s')" % (package, version, rep.repository))
        rep.checkout(package, version, vers_dir = self.options.version_dirs)
        # Call "cmt config"
        if self.options.version_dirs:
            pkgdir =  os.path.join(package, version, "cmt")
        else:
            pkgdir =  os.path.join(package, "cmt")
        self._doCMTConfig(cwd = pkgdir)
        # return the path to the cmt directory of the package to be able to call
        # a "cmt show uses" and get the packages needed with the recursion
        return (package, version, pkgdir)
    
    def checkoutProject(self, project, version):
        reps = self.projects[project]
        if len(reps) > 1:
            lst = []
            for k in reps:
                lst.append('%s (%s)' % (k, self.repositories[k]))
            idx = self._listChoice("Project '%s' in more than one repository, choose the repository." % project,
                                   lst)
        else:
            idx = 0
        rep = self.repositories[reps[idx]]
        if version != "head": # head is always valid
            versions = rep.listVersions(project, isProject = True)
            if not versions:
                raise RuntimeError("No version found for project '%s'" % project)
            if (not version) or (version not in versions):
                if version:
                    self.log.warning("Version '%s' not found for project '%s'" % (version, project))
                version = self._askVersion(versions)
        self.log.info("Checking out %s %s (from '%s')" % (project, version, rep.repository))
        rep.checkout(project, version, vers_dir = True, project = True)
        pkgdir =  os.path.join(project, "%s_%s" % (project, version))
        return (project, version, pkgdir)
        
    ## Prepare the repository access objects according to options 
    @property
    def repositories(self):
        if self._repositories is None:
            #--- Prepare repositories urls
            # filter the requested protocols for the know repositories
            repositories = {}
            for rep in __repositories__:
                reps = __repositories__[rep]
                if self.options.protocol in reps:
                    repositories[rep] = reps[self.options.protocol]
            if not repositories:
                self.log.error("Unable to find a repository for the specified protocol")
                return 1
            
            # set the usernames 
            if self.options.user and self.options.protocol != "anonymous":
                for rep in repositories.values():
                    rep.user = self.options.user
            
            # connect to repositories
            self._repositories = {}
            for rep in repositories:
                url = str(repositories[rep])
                self.log.info("Using repository '%s' for '%s'", url, rep)
                self._repositories[rep] = rcs.connect(url)
            #--- Now we are ready to talk to the repositories
        return self._repositories
    
    ## Retrieve the list of packages from the known repositories
    @property 
    def packages(self):
        if self._packages is None:
            repos = self.repositories
            if self.selected_repository:
                rep = self.selected_repository.lower()
                if rep not in self.repositories:
                    self.log.error("Uknown repository alias '%s' (allowed: %r)",
                                   self.selected_repository, self.repositories.keys())
                    return 1
                repos = { rep: self.repositories[rep] }
            
            # Collect the list of packages
            self._packages = {}
            for rep in repos:
                self.log.debug("Retrieving packages list from '%s'", rep)
                for p in repos[rep].listPackages(self.selected_hat):
                    if p in self._packages:
                        self._packages[p].append(rep)
                    else:
                        self._packages[p] = [ rep ]
        return self._packages
    
    ## Retrieve the list of packages from the known repositories
    @property 
    def projects(self):
        if self._projects is None:
            repos = self.repositories
            if self.selected_repository:
                rep = self.selected_repository.lower()
                if rep not in self.repositories:
                    self.log.error("Uknown repository alias '%s' (allowed: %r)",
                                   self.selected_repository, self.repositories.keys())
                    return 1
                repos = { rep: self.repositories[rep] }
            
            # Collect the list of packages
            self._projects = {}
            for rep in repos:
                self.log.debug("Retrieving projects list from '%s'", rep)
                for p in repos[rep].listProjects():
                    if p in self._projects:
                        self._projects[p].append(rep)
                    else:
                        self._projects[p] = [ rep ]
        return self._projects
    
    def parseOpts(self, args):
        # Replace old options '-rr' and '-rh' into the new equivalents
        for old, new in [ ('-rr', '-R'), ('-rh', '-H') ]:
            while old in args:
                args[args.index(old)] = new
        Script.parseOpts(self, args)
        # Validate and parse positional arguments
        if self.options.interactive:
            if self.options.project:
                self.parser.error("Options '-i' and '--project' cannot be used at the same time")
            # getpack.py -i [repos [hat]]
            self.selected_repository = None
            self.selected_hat = None
            if self.args:
                self.selected_repository = self.args.pop(0).lower()
                self.log.info("Query only repository '%s'", self.selected_repository)
            if self.args:
                self.selected_hat = self.args.pop(0)
                self.log.info("Search for hat '%s'", self.selected_hat)
            if self.args:
                self.parser.error("Option '-i' requires maximum 2 arguments")
        if self.options.project:
            if len(self.args) != 2:
                self.parser.error("Option '--project' requires 2 arguments")
            self.project_name, self.project_version = self.args
            # LHCb projects have uppercase names
            self.project_name = self.project_name.upper()
            # I want to use the bare version number and not the conventional one
            if self.project_version.startswith(self.project_name + "_"):
                self.project_version = self.project_version[len(self.project_name)+1:]
        else:
            # getpack.py [-u] package [version]
            if self.args:
                self.requested_package = self.args.pop(0)
                if self.args:
                    self.requested_package_version = self.args.pop(0)
                if self.args:
                    self.parser.error("requires maximum 2 arguments")
            else:
                self.parser.error("package name is required unless '-i' is used")
        
        if self.options.really_recursive or self.options.recursive_head:
            # recursion is implied by the aboves
            self.options.recursive = True
    
    def askPackage(self):
        # select package
        keys = self.packages.keys()
        keys.sort()
        message = """--------------------------------------------------------------------------------
Select the package
--------------------------------------------------------------------------------"""
        idx = self._listChoice(message, keys)
        return keys[idx]
    
    def askProject(self):
        # select package
        keys = self.projects.keys()
        keys.sort()
        message = """--------------------------------------------------------------------------------
Select the project
--------------------------------------------------------------------------------"""
        idx = self._listChoice(message, keys)
        return keys[idx]
        
    def _getNeededPackages(self, pkgdir):
        required = {}
        if self.options.recursive:
            ## @todo: --recursive is only for the first package
            self.options.recursive = False
            
            self.log.debug("Executing 'cmt show uses' in '%s'", pkgdir)
            proc = Popen(["cmt", "show", "uses"], cwd = pkgdir, stdout = PIPE, stderr = PIPE)
            out, err = proc.communicate()
            
            for l in out.splitlines():
                m = self.show_uses_regexp.match(l)
                # group("spaces") is present only if we have more than one space
                # between '#' and 'use', which implies indirect dependency 
                if m and (self.options.really_recursive or (not m.group("spaces"))):
                    pkg = m.group("package")
                    if m.group("hat"):
                        pkg = m.group("hat") + "/" + pkg
                    # do not overwrite the already found (good) versions
                    if pkg not in required or '*' in required[pkg]:
                        if self.options.recursive_head:
                            required[pkg] = "head"
                        else:
                            required[pkg] = m.group("version")
            self.log.debug("Packages required: %r" % required)
        return required
    
    def getpack(self):
        if self.requested_package and self.requested_package not in self.packages:
            self.log.error("Unknown package '%s'!", self.requested_package)
            self.requested_package = None
        if self.requested_package is None or self.requested_package not in self.packages:
            self.requested_package = self.askPackage()
        
        # Dictionaries (pkg->version) of done, skipped and to-do packages
        done_packages = {}
        skipped_packages = {}
        todo_packages = { self.requested_package: self.requested_package_version }
        
        self.log.debug("Starting processing loop")
        # process the list of pending packages
        while todo_packages:
            # get an item
            pkg, vers = todo_packages.popitem()
            # Check if we can process it
            if pkg in skipped_packages or pkg in done_packages:
                # Already processed
                continue
            elif pkg not in self.packages:
                # Not found
                skipped_packages[pkg] = vers
                continue
            # Check out the package
            done_packages[pkg] = self.checkout(pkg, vers)
            pkgdir = done_packages[pkg][2]
            # See if we need to check out something else
            todo_packages.update(self._getNeededPackages(pkgdir))
        
        print "Processed packages:"
        pkgs = done_packages.keys()
        pkgs.sort()
        for p in pkgs:
            print "\t%s\t%s" % done_packages[p][:2]
        
        if skipped_packages:
            print "Skipped packages (not in the repositories):"
            pkgs = skipped_packages.keys()
            pkgs.sort()
            for p in pkgs:
                print "\t%s" % p
    
    def getproject(self):
        if self.project_name not in self.projects:
            self.log.error("Unknown project '%s'!", self.project_name)
            self.project_name = self.askProject()
        proj = self.checkoutProject(self.project_name, self.project_version)
        if self.options.recursive:
            # get the conatiner package too, etc.
            try:
                self.requested_package = None
                for l in open(os.path.join(proj[2],"cmt","project.cmt")):
                    l = l.strip().split()
                    if len(l) == 2 and l[0] == "container":
                        self.requested_package = l[1]
                        break
                if not self.requested_package:
                    # project.cmt does not have the "container" instruction, let's assume we want ProjectSys
                    psys = proj[0].upper() + "SYS"
                    for p in self.packages:
                        if p.upper() == psys:
                            self.requested_package = p
                            break
                self.requested_package_version = proj[1] # this is the actual version extracted
                self.log.info("Retrieving packages in %s", proj[2])
                os.chdir(proj[2]) # go to the project directory
                self.log.info("Container package is %s %s", self.requested_package, self.requested_package_version)
                self.getpack()
            except IOError, x:
                self.log.error("Problems opening the 'project.cmt' file. %s", x)
        print "Checked out project %s %s in '%s'" % proj
        
    def main(self):
        try:
            if self.options.project:
                self.getproject()
            else:
                self.getpack()
        except Quit:
            print "Quit!"
        
        #from pprint import pprint
        #pprint(packages)
        return 0
