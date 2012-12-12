"""
Module providing the basic functionalities of the GetPack utility.
"""
from LbConfiguration.Repository import getRepositories
from LbUtils.Temporary import TempDir
from LbUtils.VCS import svn_repository_rexp, cvs_repository_rexp
from LbUtils.VCS import VCS_PROTOCOL, VCS_USER, VCS_HOST, VCS_PATH
from LbUtils.VCS import svn_retry_command
from LbUtils.VCS import cvs_command as _cvs
from LbUtils.VCS import splitlines

class RcsError(IOError):
    pass

class CvsError(RcsError):
    pass

class SvnError(RcsError):
    pass

def _svn(*args, **kwargs):
    out, err, ret = svn_retry_command(*args, **kwargs)
    if ret:
        raise SvnError("svn call (%s, %s) failed with retcode %d" % (args, kwargs, ret))
    return out, err, ret

import logging
log = logging.getLogger(__name__)

import os, re

import __builtin__
if "set" not in dir(__builtin__):
    # pylint: disable-msg=W0622
    from sets import Set as set

class RCSError(RuntimeError):
    """
    Generic error from revision control system.
    """

class RCSCannotHandleError(RCSError):
    """
    Error raised in case the repository cannot be handle by the instance.
    """
    def __init__(self, cls, repository):
        self.cls = cls
        self.repository = repository
    def __str__(self):
        return "Class '%(cls)s' cannot handle repository %(repository)r" % self.__dict__

class RCSUnknownModuleError(RCSError):
    """
    Error raised when the module is not found in the repository.
    """
    def __init__(self, module, repository, isProject = False):
        self.module = module
        self.repository = repository
        if isProject:
            self.module_type = "project"
        else:
            self.module_type = "package"
    def __str__(self):
        return "Cannot find %(module_type)s '%(module)s' in repository %(repository)r" % self.__dict__

class RCSUnknownVersionError(RCSError):
    """
    Error raised when the module does not have the requested version.
    """
    def __init__(self, module, version):
        self.module = module
        self.version = version
    def __str__(self):
        return "Cannot find version '%(version)s' for module '%(module)s'" % self.__dict__

_versionFilter = None
def setVersionFilter(filter_ = None):
    global _versionFilter
    if filter_ is None:
        # The default version filter_ does no check
        filter_ = lambda version: version
    _versionFilter = filter_
# Set the default version filter_
setVersionFilter()

def filterSortVersions(tags, filter_ = None):
    if filter_ == None:
        filter_ = _versionFilter
    versions = []
    for t in tags:
        version = filter_(t)
        if version is not None:
            versions.append((version, t))
    versions.sort(reverse = True)
    return [ v[1] for v in versions ]

class RevisionControlSystem(object):
    """
    Base class for interaction with a revision control system (RCS) as CVS or
    Subversion (SVN).
    """
    def __init__(self, repository):
        """
        Initialize the connection to the repository.
        The parameter 'repository' should contain enough information to connect
        to the RCS. The back-end specific implementation is responsible of
        interpreting the data.
        """
        self.repository = repository
        if not self.canHandle(repository):
            raise RCSCannotHandleError()
        self._packages = None
        self._projects = None
        self._pkg_versions = {}
        self._proj_versions = {}

    @classmethod
    def canHandle(cls, repository):
        """
        Return true if an instance of the class can handle the repository
        specified.
        A back-end specific implementation should implement this method to allow
        an automatic discovery of the correct implementation for a given RCS
        repository.
        """
        return False

    def _retrievePackages(self):
        return []

    def _retrieveProjects(self):
        return []

    @property
    def packages(self):
        if self._packages is None:
            self._packages = self._retrievePackages()
        return self._packages

    @property
    def projects(self):
        if self._projects is None:
            self._projects = self._retrieveProjects()
        return self._projects

    def listPackages(self, path = None):
        """
        Return a list of strings identifying the packages below the specified
        path (or the root of the repository if not specified).
        """
        modules = self.packages
        if path:
            # to be sure that it is the hat and not the begin of the name
            if path[-1] != "/":
                path += "/"
            modules = [ m for m in modules if m.startswith(path) ]
        return modules

    def listProjects(self):
        """
        Return a list of strings identifying the projects.
        """
        return self.projects

    def hasModule(self, module, isProject = False):
        """
        Check if the specified 'module' is available in the repository.
        """
        return (isProject and module in self.projects) or \
               ((not isProject) and module in self.packages)

    def _assertModule(self, module, isProject = False):
        """
        Raise an RCSUnknownModuleError if the module does not exist.
        """
        if not self.hasModule(module, isProject):
            raise RCSUnknownModuleError(module, self.repository, isProject)

    def _retrieveVersions(self, module, isProject):
        """
        Extract from the repository the list of symbolic names for the module.
        """
        return []

    def listVersions(self, module, isProject = False):
        """
        Return a list of strings identifying the known versions of a package.
        """
        self._assertModule(module, isProject)
        return filterSortVersions(self._retrieveVersions(module, isProject))

    def hasVersion(self, module, version, isProject = False):
        """
        Check if the specified 'version' is available for the package.
        By default this method only checks the available versions (even if not
        accepted by the 'filterSortVersions' function), including "head".
        Derived classes can override it to support specific symbolic names.
        """
        self._assertModule(module, isProject)
        return version.lower() == "head" \
            or version in self._retrieveVersions(module, isProject)

    def checkout(self, module, version = "head", dest = None, vers_dir = False,
                 project = False, eclipse = False):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used.
        """
        pass

    def tag(self, module, version, isProject = False, from_branch = None):
        """
        Create a tag for the given module.
        Retursn the error code of the underlying command.
        """
        return 0

    def move(self, package, project):
        """ Move a package from one project to another """
        return 0

    def _getRequirements(self, module, version = "head"):
        """
        Return the content of the trunk/HEAD version of the requirements/project.cmt
        file of the module for the requested version (in the repository).
        """
        return ""

    def getDeclaredVersion(self, module, version = "head"):
        """
        Return the version number that the trunk/HEAD version of the module has
        in the requirements/project.cmt file (in the repository).
        """
        requirements = self._getRequirements(module, version)
        for l in requirements.splitlines():
            l = l.strip()
            if l and l.startswith("version"):
                return l.split()[1]
        return None

    def _create_vers_cmt(self, base, version = "v*"):
        """Create the version.cmt file in 'base'/cmt.
        The file will contain the version found in the requirements file. If there is no
        version in the requirements, an already present version.cmt is kept otherwise it
        is created with the content of the parameter 'version'.
        """
        cmt_dir = os.path.join(base, "cmt")
        found_version = None
        try:
            for l in open(os.path.join(cmt_dir, "requirements")):
                l = l.split()
                if len(l) == 2 and l[0] == "version":
                    found_version = l[1]
                    break
        except IOError:
            pass # if I do not have a requirements file => we cannot find a version
        vers_file = os.path.join(cmt_dir,"version.cmt")
        if found_version:
            open(vers_file, "w").write(found_version + "\n")
        elif not os.path.exists(vers_file):
            open(vers_file, "w").write(version + "\n")

class CVS(RevisionControlSystem):
    """
    CVS implementation of RevisionControlSystem.
    """
    __repository_rexp__ = cvs_repository_rexp
    CVS_PROTOCOL = VCS_PROTOCOL
    CVS_USER     = VCS_USER
    CVS_HOST     = VCS_HOST
    CVS_PATH     = VCS_PATH
    __package_version_check_file__ = "/cmt/requirements"
    __project_version_check_file__ = "/cmt/project.cmt"
    def __init__(self, repository):
        """
        Initialize the connection to the repository.
        The parameter 'repository' should contain enough information to connect
        to the RCS. The back-end specific implementation is responsible of
        interpreting the data.
        """
        super(CVS, self).__init__(repository)
        self.protocol, self.user, self.host, self.path = \
            self.__repository_rexp__.match(repository).groups()
        self._packages = None
        self._projects = None
        self._paths = None

    @classmethod
    def canHandle(cls, repository):
        """
        Return true if an instance of the class can handle the repository
        specified.
        A back-end specific implementation should implement this method to allow
        an automatic discovery of the correct implementation for a given RCS
        repository.
        """
        return cls.__repository_rexp__.match(repository)

    def _retrieveModules(self):
        """
        Get the list of modules in the repository and separate them in projects
        and packages.
        """
        out, _err, _retcode = _cvs("-d", self.repository, "checkout", "-s")
        modules = [ l.split()
                    for l in out.splitlines()
                    if not l.startswith(" ") ]
        packages = []
        projects = []
        paths = {}
        # empiric definition:
        # a project is a module all capital letters, with no hat
        proj_re = re.compile("^[A-Z0-9_]*$")
        for m in modules:
            if len(m) > 2:
                m, path = m[0], m[2]
            else:
                m = path = m[0]
            paths[m] = path
            if proj_re.match(m) and m != "CVSROOT":
                projects.append(m)
            else:
                packages.append(m)
        self._packages = packages
        self._packages.sort()
        self._projects = projects
        self._projects.sort()
        self._paths = paths

    def _retrievePackages(self):
        """
        Return a list of strings identifying the packages below the specified
        path (or the root of the repository if not specified).
        Note that in CVS there is not a clear distinction between packages and
        projects, so a CVS module can be both.
        """
        self._retrieveModules()
        return self._packages

    def _retrieveProjects(self):
        """
        Return a list of strings identifying the projects.
        Note that in CVS there is not a clear distinction between packages and
        projects, so a CVS module can be both.
        """
        self._retrieveModules()
        return self._projects

    def _retrieveVersions(self, module, isProject):
        """
        Extract from the repository the list of symbolic names for the module.
        """
        # Get the log of the requirements file
        module_dir =  self._paths[module]
        file_to_check = self.__package_version_check_file__
        if isProject:
            file_to_check = self.__project_version_check_file__

        out, _err, _retcode = _cvs("-d", self.repository, "rlog", module_dir + file_to_check)
        # extract the list of tags
        tags = []
        outl = out.splitlines()
        while outl: # remove the lines until "symbolic names:" (included)
            l = outl.pop(0)
            if l.startswith("symbolic names:"):
                break
        for l in outl: # get the tag names
            if l and l[0] == '\t':
                # the tag name looks like '\tvXrY: #.#'
                tag = l.strip().split(":")[0]
                if isProject and tag.startswith(module + '_'):
                    # remove the 'PROJECT_' from the tag
                    tag = tag[len(module)+1:]
                tags.append(tag)
            else:
                break
        return tags

    def checkout(self, module, version = "head", dest = None, vers_dir = False,
                 project = False, eclipse = False):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used.
        """
        # check for the validity of the version
        # (this implies a check on the existence of the module)
        if not self.hasVersion(module, version, project):
            raise RCSUnknownVersionError(module, version)

        if dest: # prepare destination directory
            if not os.path.isdir(dest):
                os.makedirs(dest, mode = 0755)
            os.chdir(dest)
        else:
            dest = "." # If not specified, use local directory

        # prepare options for CVS checkout
        options = ("-d", self.repository, "checkout", "-P")
        isHead = version.lower() in ["head", "trunk"]
        if project:
            version = "%s_%s" % (module, version)
        if not isHead:
            options += ("-r", version)
        if vers_dir:
            options += ("-d", "/".join([module, version]))
        options += (module,)
        apply(_cvs, options, {"cwd": dest, "stdout": None, "stderr": None})

        if not vers_dir and not project:
            # create version.cmt file
            self._create_vers_cmt(os.path.join(dest, module), version)

    def tag(self, module, version, isProject = False, from_branch = None):
        """
        Create a tag for the given module.
        """
        self._assertModule(module, isProject)
        options = ("-d", self.repository, "rtag", version, module)
        _, _, retcode = apply(_cvs, options, {"stdout": None, "stderr": None})
        return retcode

    def move(self, package, project):
        """ Move a package from one project to another """
        log.error("The move action is not supported on CVS repositories")
        return 1

    def _getRequirements(self, module, version = "head"):
        """
        Return the content of the trunk/HEAD version of the requirements/project.cmt
        file of the module for the requested version (in the repository).
        """
        self._assertModule(module, isProject = False)
        module_dir =  self._paths[module]
        file_to_check = self.__package_version_check_file__
        if version.lower() == "head":
            version = "HEAD"
        out, _, _ = _cvs("-d", self.repository, "co", "-p", "-r", version, module_dir + file_to_check)
        return out

    ## Human-readable description
    def __str__(self):
        return "CVS repository at %s" % self.repository



class SubversionCmd(RevisionControlSystem):
    """
    SVN implementation of RevisionControlSystem.
    """
    __repository_rexp__ = svn_repository_rexp
    SVN_PROTOCOL = VCS_PROTOCOL
    SVN_USER     = VCS_USER
    SVN_HOST     = VCS_HOST
    SVN_PATH     = VCS_PATH
    PROJECT_NODES = ['cmt', 'cmake', 'CMakeLists.txt', 'toolchain.cmake',
                     'Makefile.cmt', 'configure']

    REVID_EXP = re.compile(r'^r([0-9]+)$')
    DATEID_EXP = re.compile(r'^(\{\d{4}-\d{2}-\d{2}(( \d{2}:\d{2})|(T\d{2}:\d{2}Z?))\})$')

    def __init__(self, repository):
        """
        Initialize the connection to the repository.
        The parameter 'repository' should contain enough information to connect
        to the RCS. The back-end specific implementation is responsible of
        interpreting the data.
        """
        super(SubversionCmd, self).__init__(repository)
        self.protocol, self.user, self.host, self.path = \
            self.__repository_rexp__.match(repository).groups()
        # cache of module names and versions
        self._modules = None # will be a dictionary
        self._packages = None
        self._repositoryVersion = None
        self._ls_cache = {}
        self._latestRevisionCached = None
        self.forceScan = False
        self.useBranches = False

    @classmethod
    def canHandle(cls, repository):
        """
        Return true if an instance of the class can handle the repository
        specified.
        A back-end specific implementation should implement this method to allow
        an automatic discovery of the correct implementation for a given RCS
        repository.
        """
        m = cls.__repository_rexp__.match(repository)
        # if protocol is "file" we do not want the host and vice versa
        valid = m and \
                ( ( m.group(cls.SVN_PROTOCOL) == "file" and not m.group(cls.SVN_HOST) ) or \
                  ( m.group(cls.SVN_PROTOCOL) != "file" and m.group(cls.SVN_HOST) ) )
        return valid

    def _ls(self, path):
        """
        List the content of a directory or a list of directories in the repository.
        """
        # Strip the leading and trailing "/" for consistency
        path = path.strip("/")
        # remove the leading repository name, if present
        if path.startswith(self.repository):
            path = path[len(self.repository):]
        # Rationalize "//" for consistency
        while "//" in path:
            path = path.replace("//", "/")
        if path not in self._ls_cache:
            out, _err, _retcode = _svn("ls", "/".join([self.repository, path]))
            if _retcode:
                raise IOError(_err)
            # @TODO add error-checking
            self._ls_cache[path] = out.splitlines()
        return self._ls_cache[path]

    def _exists(self, path):
        """
        Function provided for readability.
        """
        # Use efficiently the _ls_cache
        path = path.strip("/") # the paths are used without the leading and trailing '/'
        if path in self._ls_cache: # check if the path is in the cache
            return True
        if "/" in path: # if the path has more than one level, try to use the cache for the parent path
            d, b = path.rsplit("/", 1) # separate dirname and basename
            if d in self._ls_cache:
                l = self._ls_cache[d]
                if (b in l) or ((b+'/') in l): # allow for files and dirs
                    return True
        # If path and dirname(path) are not in the cache, look for path in the repository
        try:
            self._ls(path)
            return True
        except IOError:
            return False

    def _walk(self, path = ""):
        entries = self._ls(path)
        dirs = []
        files = []
        for l in entries:
            if l.endswith("/"):
                dirs.append(l[:-1])
            else:
                files.append(l)
        yield path[:], dirs, files
        for d in dirs:
            for w in self._walk("/".join([path, d])):
                yield w

    def _find(self, left, right = "", levels = 1, exists_only = False):
        """
        Return a list of valid values of x for x such that
        (left + '/' + x + '/' right) exists.
        The parameter levels is used to select the depth of the recursion (how
        many levels must be included in x).
        If the parameter exists_only is set to True, the function returns only
        the first match, for the cases where only a match is requested and not
        the complete list.
        """
        if not left.endswith("/"):
            left += "/"
        # Take only the directories
        candidates = filter(lambda x: x.endswith("/"), self._ls(left))
        if levels > 1:
            matches = []
            for c in candidates:
                for m in self._find(left + c, right, levels - 1, exists_only):
                    matches.append(c + m)
                    if exists_only:
                        # we need only one successful match
                        return matches
        else:
            if right:
                matches = []
                for x in candidates:
                    if self._exists(left + x + right):
                        matches.append(x)
                        if exists_only:
                            # we need only one successful match
                            return matches
            else:
                # no need to check for exists_only because we cannot do less than "ls"
                matches = candidates
        return matches

    def _propGet(self, property_name, path = None):
        """
        Returns the value of the requested property for the specified path.
        If no path is specified, the root of the repository is used.
        """
        if path:
            if path[0] == "/": # we need relative path
                path = path[1:]
            path = "/".join([self.repository, path])
        else:
            path = self.repository
        out, _err, _retcode = _svn("propget", property_name, path)
        return out

    @property
    def repositoryVersion(self):
        """
        Return the version of the repository structuring as a tuple.
        """
        if self._repositoryVersion is None:
            rv = self._propGet("version")
            if rv:
                self._repositoryVersion = tuple(rv.strip().split("."))
            else:
                self._repositoryVersion = (1,0)
        return self._repositoryVersion

    def _getModulesFromProp(self):
        """
        Return the content of the modules property of the 'packages' directory as a list.
        """
        modules = {}

        if self.forceScan: # do not check the content of the property
            return modules

        if self.repositoryVersion < (2,0):
            out = self._propGet("modules", "packages")
            for m in out.splitlines():
                if m:
                    modules[m] = None
        else:
            for l in splitlines(self._propGet("packages", "")):
                package, project = l.split()[:2]
                modules[package] = project
        return modules

    def _scanModules(self):
        """
        Scan the directory structure to find modules.
        """
        # full scan of the repository (inefficient)
        modules = {}
        if self.repositoryVersion < (2,0):
            for path, dirs, _files in self._walk("packages"):
                if "trunk" in dirs:
                    dirs[:] = [] # do not recurse further
                    modules["/".join(path.split("/")[1:])] = ()
        else:
            # In repository >= 2.0 the packages are distributed in the various
            # projects and, if dropped from the release, only in the tags directory
            #
            # ProjectA/trunk/PackageA1
            #               /PackageA2
            #         /tags/PackageA0/vXrY
            # ProjectB/trunk/PackageB1
            #               /PackageB2
            #               /Hat/PackageB3
            #         /tags/Hat/PackageB0/vXrY
            #
            # The only sensible way to locate the packages is to loop in all the
            # trunk/tags/branches directories, identifying them by the presence
            # of "cmt/requirements". A possible way of optimizing the process
            # can be:
            #  - first look into _all_ the trunks directories (less noise)
            #  - try all tags and branches directories, skipping already
            #    discovered packages and stopping at the first positive match.
            #  - we can put ranges to the depth of the scanning, something like:
            #     - "Proj/trunk/<pack>/cmt/requirements"
            #     - "Proj/trunk/<hat>/<pack>/cmt/requirements"
            #     - "Proj/tags/<pack>/v*/cmt/requirements"
            #     - "Proj/tags/<hat>/<pack>/v*/cmt/requirements"
            # It should be decided if the projects tags and packages tags have
            # to be in the same directory or not.
            for p in self.projects:
                P = p.upper() + '/'
                # look in the trunk for normal packages
                for m in self._find("/".join([p,"trunk"]), "cmt/requirements", levels = 1):
                    modules[m] = p
                # and packages with hat
                for h in [ h
                           for h in self._ls("/".join([p,"trunk"]))
                           if h.endswith("/") and h not in modules ]:
                    for m in self._find("/".join([p,"trunk",h]), "cmt/requirements", levels = 1):
                        modules[h + m] = p
                # look in the tags directories for dismissed packages
                # NOTE: branches not supported... not sure it is needed
                #for d in ["tags", "branches"]:
                for d in ["tags"]:
                    # first level candidates:
                    #   directories in Proj/tags, not included in modules and that have at least
                    #   one */cmt/requirements inside
                    for m in [ c
                               for c in self._ls("/".join([p,d]))
                               if c.endswith("/") and c != P and c not in modules and
                                   self._find("/".join([p,d,c]), "cmt/requirements",
                                              levels = 1, exists_only = True)]:
                        modules[m] = p
                    # second level candidates:
                    #  in each hat (directory in Proj/tags no included in modules and different from "PROJECT/")
                    for h in [ h
                               for h in self._ls("/".join([p,d]))
                               if h.endswith("/") and h != P and h not in modules ]:
                        for m in [ c
                                   for c in [ h+c for c in self._ls("/".join([p,d,h]))]
                                   if c.endswith("/") and c not in modules and
                                       self._find("/".join([p,d,c]), "cmt/requirements",
                                                  levels = 1, exists_only = True)]:
                            modules[m] = p
            # FIXME: We should look for packages in the projects tags too

            # remove trailing "/" from keys
            tmp = {}
            for m in modules:
                tmp[m[:-1]] = modules[m]
            modules = tmp
        return modules

    @property
    def modules(self):
        if self._modules is None:
            # if _getModulesFromProp is empty, try _scanModules
            self._modules = self._getModulesFromProp() or self._scanModules()
        return self._modules

    def _retrievePackages(self):
        # Delegate to the modules property
        packs = self.modules.keys()
        packs.sort()
        return packs

    def _retrieveProjects(self):
        if self.repositoryVersion < (2,0):
            return [ l[:-1] for l in self._ls("projects") if l.endswith("/") ]
        else:
            # In repository >= 2.0 the projects are at the top level of
            # the repository... How to store other informations? Like "docs", "www"...
            # Probably we should take an approach similar to the one of packages:
            # register the list of projects in a top-level property, with the
            # possibility of scanning the repository (using "cmt/project.cmt"
            # as signature, may be both in trunk and tags)

            # First try to read the "projects" property if not disabled
            if not self.forceScan:
                projects = splitlines(self._propGet("projects", ""))
                if projects:
                    return projects

            # all the projects that have an "active" development
            projects = self._find("", "trunk/cmt/project.cmt")

            # projects that exist only in tags or branches:
            candidates = [ p
                           for p in self._ls("")
                           if p.endswith("/") and p not in projects ]
            for c in candidates:
                # NOTE: branches not supported... no need to look for projects in branches
                #if self._find(c + "tags/" + c, "cmt/project.cmt", exists_only = True) or \
                #    self._find(c + "branches/" + c, "cmt/project.cmt", exists_only = True):
                if self._find(c + "tags/" + c.upper(), "cmt/project.cmt", exists_only = True):
                    projects.append(c)
            # Return a list with the trailing "/" stripped
            return [ p[:-1] for p in projects ]

    def _topLevel(self, isProject = False):
        if isProject:
            return "projects"
        else:
            return "packages"

    def _latestRevision(self):
        """
        Return the latest revision number available in the repository.
        """
        if self._latestRevisionCached is None:
            out, _err, _retcode = _svn("info", "--xml", self.repository)
            from xml.etree import ElementTree
            et = ElementTree.fromstring(out)
            self._latestRevisionCached = int(et.find('entry').get('revision', 0))
        return self._latestRevisionCached

    def _retrieveVersions(self, module, isProject):
        """
        Extract from the repository the list of symbolic names for the module.
        """
        if self.repositoryVersion < (2,0):
            return [ l[:-1]
                     for l in self._ls("%s/%s/tags" % (self._topLevel(isProject), module))
                     if l.endswith("/") ]
        else:
            if isProject:
                p = module
                P = p.upper()
                P_ = P + "_"
                vers= []
                tag_ls = self._ls("/".join([p,"tags",P]))
                try:
                    if self.useBranches:
                        tag_ls.extend(self._ls("/".join([p,"branches",P])))
                except SvnError:
                    # ignore missing branches directory
                    log.warning('project %s does not have the "branches" directory', p)
                for v in [ l[:-1]
                           for l in tag_ls
                           if l.endswith("/") ]:
                    if v.startswith(P_):
                        v = v[len(P_):]
                    vers.append(v)
                return vers
            else:
                versions = set()
                if module in self.modules:
                    proj = self.modules[module]
                    versions.update(self._find("/".join([proj,"tags",module])))
                    if self.useBranches:
                        try:
                            versions.update(self._find("/".join([proj,"branches",module])))
                        except SvnError:
                            # ignore missing branches directory
                            log.warning('package %s does not have the "branches" directory', module)
                    # FIXME: the project tags for the packages are not supported (yet) by GetPack.py
                    #versions.update(self._find("/".join([proj,"tags",proj.upper()]), module))
                    #versions.update(self._find("/".join([proj,"branches",proj.upper()]), module))
                versions = [ v[:-1] for v in versions ] # remove trailing "/" and convert to list
                return versions

    def isTag(self, module, version, isProject):
        """
        Returns true if the specified version exists and refers to a tag.
        """
        return self._exists(self.url(module, version, isProject, branch=False))

    def isBranch(self, module, version, isProject):
        """
        Returns true if the specified version exists and refers to a branch.
        """
        return self._exists(self.url(module, version, isProject, branch=True))

    def hasVersion(self, module, version, isProject = False):
        """
        Check if the specified 'version' is available for the package.
        By default this method only checks the available versions (even if not
        accepted by the 'filterSortVersions' function, but derived classes can
        override it to support specific symbolic names.
        """
        self._assertModule(module, isProject)
        if version == "trunk":
            return True
        m = self.REVID_EXP.match(version)
        if m and int(m.group(1)) <= self._latestRevision():
            return True
        # check if it is a date
        if self.DATEID_EXP.match(version):
            return True
        return super(SubversionCmd, self).hasVersion(module, version, isProject)

    def _computePaths(self, module, version, isProject = False, vers_dir = False, global_tag = False, branch = False):
        """
        Return the URL to the requested version of the module and the (relative)
        path that should be used for the check out.
        """
        if self.repositoryVersion < (2, 0):
            if version.lower() not in ["head", "trunk"]:
                src = "/".join([self.repository, self._topLevel(isProject), module, "tags", version])
            else:
                src = "/".join([self.repository, self._topLevel(isProject), module, "trunk"])
            if isProject:
                version = "%s_%s" % (module, version)
            dst = module
            if vers_dir:
                dst = os.path.join(dst, version)
        else:
            if isProject:
                root = module
                module = module.upper()
                versiondir = "%s_%s" % (module, version)
            else:
                versiondir = version
                root = self.modules[module]
            # Compare the version string with the revision ID regexp
            revId = self.REVID_EXP.match(version) or self.DATEID_EXP.match(version)
            if revId or version.lower() in ["head", "trunk"]: # a revId or trunk (or head) are checked out from trunk
                src = [root, "trunk"]
                if not isProject:
                    src.append(module)
            else:
                sub = "tags"
                if branch:
                    sub = "branches"
                src = [root, sub, module, versiondir]
            src = "/".join([self.repository] + src)
            if revId: # Special syntax for revision IDs
                src += "@" + revId.group(1)
            dst = module
            if vers_dir:
                dst = os.path.join(dst, versiondir)
        return src, dst


    def _getPackagePath(self, module, key="trunk"):
        result = None, None
        root = self.modules[module]
        purl = "/".join([self.repository, root, key, module])
        pdir = os.path.join(root, key, module)
        if self._exists(pdir) :
            result = (purl, pdir)
        else :
            log.debug("%s doesn't exist" % purl)
        return result

    def _getAllPackagePaths(self, module):
        pkg_paths = []
        for m in ["trunk", "tags", "branches"] :
            purl, pdir = self._getPackagePath(module, m)
            if pdir :
                pkg_paths.append((purl, pdir))
        return pkg_paths



    def _getProjectPath(self, module, key="trunk"):
        result = None, None
        root = module
        purl = "/".join([self.repository, root, key])
        pdir = os.path.join(root, key)
        if self._exists(pdir) :
            result = (purl, pdir)
        else :
            log.debug("%s doesn't exist" % purl)
        return result

    def _getAllProjectPaths(self, module):
        prj_paths = []
        for m in ["trunk", "tags", "branches"] :
            purl, pdir = self._getProjectPath(module, m)
            if pdir :
                prj_paths.append((purl, pdir))
        return prj_paths


    def url(self, module, version = "trunk", isProject = False, branch = False):
        """
        Return the full URL to the module in the repository.
        """
        return self._computePaths(module, version, isProject, branch=branch)[0]

    def checkout(self, module, version = "head", dest = None, vers_dir = False,
                 project = False, eclipse = False, global_tag = False, ifExistsAction=None):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used.

        @param module: name of the module
        @param version: version of the module
        @param dest: destination directory (default ".")
        @param vers_dir: if the version directory must be created
        @param project: if the module name refers to a project
        @param eclipse: if the eclipse configuration must be created (by default
                        it is created only if in an eclipse workspace)
        @param global_tag: if True and project == True, check out the complete
                           project tag, otherwise only the 'cmt' directory
        @param ifExistsAction: what to do if the directory already exists, usually nothing
                               but maybe try --relocate first if protocols are not equal
        """
        from os.path import exists, join, dirname, abspath, isdir
        # check for the validity of the version
        # (this implies a check on the existence of the module)
        if not self.hasVersion(module, version, project):
            raise RCSUnknownVersionError(module, version)
        branch = self.isBranch(module, version, project)

        if dest: # prepare destination directory
            if not os.path.isdir(dest):
                os.makedirs(dest, mode = 0755)
            os.chdir(dest)
        else:
            dest = "." # If not specified, use local directory
        # we should create Eclipse-friendly configuration if we are in an Eclipse
        # workspace (i.e. exists(<dest>/../.metadata) or if we are asked explicitly
        eclipse = eclipse or exists(join(dirname(abspath(dest)), ".metadata"))

        src, dst = self._computePaths(module, version, project, vers_dir, global_tag, branch=branch)

        # Check if we can do an advanced (eclipse-friendly checkout)
        # - first prepare the top level directory if needed
        if not project and not vers_dir: # we can do it on packages without version
            if eclipse and not exists(join(dest, ".svn")):
                # there is no svn directory, make a checkout of the project
                # containing the package
                psrc, _ = self._computePaths(self.modules[module], "trunk", isProject=True)
                _svn("co", "-N", psrc, dest, stdout = None, stderr = None)
                # top level directory ready
        # - check if we can use the top level directory
        if exists(join(dest, ".svn")):
            out = _svn("info", dest, stderr = None)[0]
            try:
                url = (l for l in out.splitlines() if l.startswith("URL:")).next().split()[-1]
                # check if the trunk (or branch) directory of the package is the same as the top dir
                if src.replace("tags", "trunk").replace("branches", "trunk").startswith(url):
                    # the package is in the same project dir
                    ldst = dst.replace("\\", "/").split("/")
                    step = dest
                    while ldst:
                        step = join(step, ldst.pop(0))
                        if ldst or project:
                            # non-recursive checkout for the intermediate levels
                            # (for projects we need to get to the last element)
                            if _svn("up", "-N", step)[2]:
                                break
                        else:
                            # full check-out for the final one
                            _svn("up", step, stdout = None)
            except:
                # ignore errors
                pass

        # full path to destination
        dst = join(dest, dst)

        if isdir(join(dst,".svn")): # looks like a SVN working copy
            # try with "switch"
            if ifExistsAction is not None:
                ifExistsAction(dst)
            sub_cmd = "switch"
        else:
            # normal case
            sub_cmd = "checkout"

        if not project:
            _svn(sub_cmd, src, dst, stdout = None, stderr = None)
        else: # Projects can come with CMake configuration files
            _svn(sub_cmd, '-N', src, dst, stdout = None, stderr = None)
            # List of subdirectories to check-out too
            for subdir in ['cmt', 'cmake']:
                if self._exists(src + '/' + subdir):
                    _svn('up', join(dst, subdir), stdout = None, stderr = None)

        if not vers_dir and not project:
            # create version.cmt file
            self._create_vers_cmt(join(dest, module), version)

    def tag(self, module, version, isProject=False, from_branch=None):
        """
        Create a tag for the given module.
        """
        if self.repositoryVersion < (2,0):
            raise RCSError("tag not implemented for Subversion repositories with "
                           "version < 2.0 (current version %s)" % self.repositoryVersion)
        self._assertModule(module, isProject)
        branch = from_branch or "trunk"
        srcUrl = self.url(module, branch, isProject, from_branch)
        versionUrl = self.url(module, version, isProject)
        msg = "Tagging %s %s as %s" % ({True: "project",
                                        False: "package"}[bool(isProject)],
                                       module, version)
        if isProject: # check if the
            if not self._exists(versionUrl):
                _svn("mkdir", "--parents", "-m", msg, versionUrl)
                # ignore errors
            # For projects we need to copy a few dirs and files
            for subnode in self.PROJECT_NODES:
                src = srcUrl + '/' + subnode
                if self._exists(src):
                    _, _, retcode = _svn("copy", "-m", msg,
                                         src, versionUrl + '/' + subnode,
                                         stdout = None, stderr = None)
                    # bail out on error
                    if retcode:
                        return retcode
            return 0
        else:
            _, _, retcode = _svn("copy", "-m", msg,
                                 srcUrl, versionUrl, stdout = None, stderr = None)
        return retcode

    def branch(self, module, version, isProject=False, from_tag=None, from_branch=None):
        """
        Create a branch for the given module.
        """
        if self.repositoryVersion < (2,0):
            raise RCSError("tag not implemented for Subversion repositories with "
                           "version < 2.0 (current version %s)" % self.repositoryVersion)
        self._assertModule(module, isProject)
        if from_tag:
            srcUrl = self.url(module, from_tag, isProject)
        else:
            branch = from_branch or "trunk"
            srcUrl = self.url(module, branch, isProject, from_branch)
        versionUrl = self.url(module, version, isProject, True)
        msg = "Branching %s %s as %s" % ({True: "project",
                                          False: "package"}[bool(isProject)],
                                         module, version)
        if isProject:
            if not self._exists(versionUrl):
                _svn("mkdir", "--parents", "-m", msg, versionUrl)
                # ignore errors
            # For projects we need to copy a few dirs and files
            for subnode in self.PROJECT_NODES:
                src = srcUrl + '/' + subnode
                if self._exists(src):
                    _, _, retcode = _svn("copy", "-m", msg,
                                         src, versionUrl + '/' + subnode,
                                         stdout = None, stderr = None)
                    # bail out on error
                    if retcode:
                        return retcode
            return 0
        else:
            _, _, retcode = _svn("copy", "-m", msg,
                                 srcUrl, versionUrl, stdout = None, stderr = None)
            return retcode

    def _updatePath(self, pth, root_dir):
        pthlist = []
        for p in pth.split("/") :
            pthlist.append(p)
            _svn("update", "-N", os.sep.join(pthlist), cwd=root_dir)

    def _createPath(self, pth, root_dir):
        pthlist = []
        for p in pth.split("/") :
            pthlist.append(p)
            if not self._exists("/".join(pthlist)) :
                _svn("mkdir", os.sep.join(pthlist), cwd=root_dir)
            else :
                _svn("update", "-N", os.sep.join(pthlist), cwd=root_dir)

    def _getPackageProperty(self):
        packprop = "### Package Project\n"
        formt = "%%-%ds%%s\n" % (max([len(x) for x in self.modules.keys()]) + 5)
        pairs = sorted([(proj, pack) for pack, proj in self.modules.items()])
        prev = None
        for proj, pack in pairs:
            if proj != prev :
                packprop += "#\n"
                prev = proj
            packprop += formt % (pack, proj)
        return packprop

    def move(self, package, project):
        """ move a package from one project to another """
        status = 0
        if package not in self.packages :
            status = 1
            log.error("The package %s is not in the %s repository" % (package, self))
        elif project not in self.projects + [ "packages", "obsolete" ] :
            status = 1
            log.error("The project %s is not in the %s repository" % (project, self) )
        elif self.modules[package] == project :
            log.warning("The package %s is already in the %s project. Nothing to do" % (package, project))
        else :
            tmpdir = TempDir(suffix="tempdir", prefix="movepak_%s_%s" % (package.replace("/", "_"), project))
            tmpdir_name = tmpdir.getName()
            log.debug("The temporary directory used is %s" % tmpdir_name)
            _, _, _ = _svn("checkout", "-N", self.repository, cwd=tmpdir_name, report_failure=True)
            root_dir  = os.path.join(tmpdir_name, self.repository.split("/")[-1])

            # package full checkout
            for k in ["trunk", "tags", "branches"] :
                _, pdir = self._getPackagePath(package, k)
                if pdir:
                    _, prdir = self._getProjectPath(project, k)
                    ppdir = os.sep.join([prdir] + package.split("/"))
                    if self._exists(ppdir) :
                        status = 1
                        log.fatal("%s directory already exists" % ppdir)
                    else :
                        # get a sparse local copy of the original package
                        log.info("Updating the path %s" % pdir)
                        self._updatePath(pdir, root_dir)

                        # prepare the destination directory
                        ppdir = prdir
                        pkghat = package.split("/")[:-1]
                        if pkghat:
                            ppdir = os.sep.join([prdir] + pkghat)
                        log.info("Creating the path %s" % ppdir)
                        self._createPath(ppdir, root_dir)

                        # do the move
                        log.info("Moving %s to %s" %(pdir, ppdir))
                        _, _, _ = _svn("move", pdir, ppdir, cwd=root_dir, report_failure=True)
                else:
                    log.warning("Could not find %s for %s, the commit will fail.", k, package)

            self.modules[package] = project
            modlist = os.path.join(tmpdir_name,"packages.list")
            f = open(modlist, "w")
            f.write(self._getPackageProperty())
            f.close()
            log.info("Setting the new packages property")
            _svn("propset", "--file", modlist, "packages", root_dir, report_failure=True)
            log.info("Committing the changes.")
            _, _, status = _svn("commit", "-m", "move_package: moved the %s package to the %s project" % (package, project), cwd=root_dir, report_failure=True)

        return status

    def _getRequirements(self, module, version = "head"):
        """
        Return the content of the trunk/HEAD version of the requirements/project.cmt
        file of the module for the requested version (in the repository).
        """
        self._assertModule(module, isProject = False)
        if version is None:
            version = "trunk"
            branch = False
        else:
            branch = self.isBranch(module, version, isProject=False)
        url = self.url(module, version, isProject=False, branch=branch)
        url += "/cmt/requirements"
        out, _, _ = _svn("cat", url, stderr = None)
        return out

    ## Human-readable description
    def __str__(self):
        return "Subversion repository at %s" % self.repository

# get all the implementations of RevisionControlSystem available in the module
__all_implementations__ = []
for _n in dir():
    if _n.startswith("_"):
        continue
    cl = eval(_n)
    if hasattr(cl,"canHandle"):
        __all_implementations__.append(cl)
del _n, cl

def connect(repository):
    for impl in __all_implementations__:
        if impl.canHandle(repository):
            return impl(repository)
    return None


def getPackageRepo(package, reps, exclude=None):
    if exclude is None :
        exclude = []
    for name in reps:
        if name not in exclude:
            url = str(reps[name]) # the protocol is forced (anyway we need to write)
            log.info("Looking for package '%s' in '%s' (%s)", package, name, url)
            repo = connect(url)
            if package in repo.packages:
                log.debug("Found package %s in %s" % (package, name))
                yield repo


def getProjectRepo(project, reps, exclude=None, extended=False):
    if exclude is None :
        exclude = []
    for name in reps:
        if name not in exclude:
            url = str(reps[name]) # the protocol is forced (anyway we need to write)
            log.info("Looking for project '%s' in '%s' (%s)", project, name, url)
            repo = connect(url)
            projs = repo.projects
            if extended and name == "lbsvn" :
                projs += [ "packages", "obsolete" ]
            if project in projs:
                log.debug("Found project %s in %s" % (project, name))
                yield repo

def moveSVNPackage(package, project, user_repos):
    status = 0
    reps = getRepositories(user_repos, protocol="ssh")
    repo_list = list(getPackageRepo(package, reps))
    if repo_list :
        repo = repo_list[0]
    else :
        repo = None
        log.fatal("No Repository found for %s" % package)
        status = 1
        return status
    proj_repo_list = list(getProjectRepo(project, reps, extended=True))
    if proj_repo_list :
        proj_repo = proj_repo_list[0]
    else :
        proj_repo = None
        log.fatal("No Repository found for %s" % project)
        status = 1
        return status
    if repo.repository != proj_repo.repository :
        log.fatal("%s is not in the same SVN repository as %s" % (package, project))
        log.debug("%s : %s %s : %s" % (package, repo, project, proj_repo))
        status = 1
        return status
    else :
        status = repo.move(package, project)
    return status
