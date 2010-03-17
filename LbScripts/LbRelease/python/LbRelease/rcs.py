"""
Module providing the basic functionalities of the GetPack utility.
"""
import os, re, sys
from subprocess import Popen, PIPE
import logging

_use_shell = sys.platform.startswith("win")
_call_command_log = logging.getLogger("_call_command")
def _call_command(cmd, *args, **kwargs):
    """
    Simple wrapper to execute a command and return standard output, standard error and return code.
    """
    d = {"stdout": PIPE, "stderr": PIPE, "shell": _use_shell}
    d.update(kwargs)
    cmd = [cmd] + list(args)
    _call_command_log.debug("Execute command: %r %r", " ".join(cmd), kwargs)
    proc = apply(Popen, (cmd,), d)
    out, err = proc.communicate()
    return (out, err, proc.returncode)

class _retry_command(object):
    """Small wrapper to add a 're-try' feature to _call_command."""
    def __init__(self, command, check, retries = 3, sleeptime = 30):
        self._command = command
        self._check = check
        self._retries = retries
        self._sleeptime = sleeptime
    def __call__(self, *args, **kwargs):
        from time import sleep
        retries = self._retries
        retval = apply(self._command, args, kwargs)
        while (not self._check(retval)) and (retries > 0):
            retval = apply(self._command, args, kwargs)
            retries -= 1
            sleep(self._sleeptime)
        return retval

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
def setVersionFilter(filter = None):
    global _versionFilter
    if filter is None:
        # The default version filter does no check
        filter = lambda version: version
    _versionFilter = filter
# Set the default version filter
setVersionFilter()

def filterSortVersions(tags, filter = None):
    if filter == None:
        filter = _versionFilter
    versions = []
    for t in tags:
        version = filter(t)
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

    def checkout(self, module, dest = None, vers_dir = False, project = False):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used.
        """
        pass
    
    def tag(self, module, version, isProject = False):
        """
        Create a tag for the given module.
        Retursn the error code of the underlying command.
        """
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

_cvs = lambda *args, **kwargs: apply(_call_command, ("cvs",) + args, kwargs)
_cvs_retry = _retry_command(_cvs, lambda r: r[2] == 0)
class CVS(RevisionControlSystem):
    """
    CVS implementation of RevisionControlSystem.
    """
    __repository_rexp__ = re.compile(r":([pkg]server|ext):(?:([\w.]+)@)?([\w.]+):(/[\w./]+)*")
    CVS_PROTOCOL = 1
    CVS_USER     = 2
    CVS_HOST     = 3
    CVS_PATH     = 4
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

    def checkout(self, module, version = "head", dest = None, vers_dir = False, project = False):
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
        isHead = version.lower() == "head"
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
    
    def tag(self, module, version, isProject = False):
        """
        Create a tag for the given module.
        """
        self._assertModule(module, isProject)
        options = ("-d", self.repository, "rtag", version, module)
        _, _, retcode = apply(_cvs, options, {"stdout": None, "stderr": None})
        return retcode
    
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

def splitlines(text):
    """
    Split a text in lines removing empty lines and lines starting with "#".
    """
    return [ l
             for l in [ l.strip() for l in text.splitlines() ]
             if l and l[0] != "#" ]

_svn = lambda *args, **kwargs: apply(_call_command, ("svn",) + args, kwargs)
_svn_retry = _retry_command(_svn, lambda r: r[2] == 0)
class SubversionCmd(RevisionControlSystem):
    """
    SVN implementation of RevisionControlSystem.
    """
    __repository_rexp__ = re.compile(r"(svn(?:\+ssh)?|https?|file)://(?:([\w.]+)@)?([\w.]*)(/[\w./]+)*")
    SVN_PROTOCOL = 1
    SVN_USER     = 2
    SVN_HOST     = 3
    SVN_PATH     = 4
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
        # Rationalize "//" for consistency
        while "//" in path:
            path = path.replace("//", "/")
        if path not in self._ls_cache:
            out, _err, _retcode = _svn("ls", "/".join([self.repository, path]))
            # @TODO add error-checking
            self._ls_cache[path] = out.splitlines()
        return self._ls_cache[path]

    def _exists(self, path):
        """
        Function provided for readability.
        """
        return self._ls(path)

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
    
    def _propGet(self, property, path = None):
        """
        Returns the value of the requested property for the specified path.
        If no path is specified, the root of the repository is used.
        """
        if path:
            if path[0] == "/": del path[0] # we need relative path
            path = "/".join([self.repository, path])
        else:
            path = self.repository
        out, _err, _retcode = _svn("propget", property, path)
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
                # FIXME: branches not supported
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
                # FIXME: branches not supported
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
                for v in [ l[:-1]
                          # FIXME: branches not supported
                          #for l in self._ls("/".join([p,"tags",P])) + self._ls("/".join([p,"branches",P]))
                          for l in self._ls("/".join([p,"tags",P]))
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
                    # FIXME: branches not supported
                    #versions.update(self._find("/".join([proj,"branches",module])))
                    # FIXME: the project tags for the packages are not supported (yet) by GetPack.py
                    #versions.update(self._find("/".join([proj,"tags",proj.upper()]), module))
                    #versions.update(self._find("/".join([proj,"branches",proj.upper()]), module))
                versions = [ v[:-1] for v in versions ] # remove trailing "/" and convert to list
                return versions

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
        m = re.match("^r([0-9]+)$", version)
        if m and int(m.group(1)) <= self._latestRevision():
            return True
        return super(SubversionCmd, self).hasVersion(module, version, isProject)

    def _computePaths(self, module, version, isProject = False, vers_dir = False):
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
            if isProject: # projects in SVN _are_ the content of the "cmt" directory
                dst = os.path.join(dst, "cmt")
        else:
            if isProject:
                root = module
                module = module.upper()
                versiondir = "%s_%s" % (module, version)
            else:
                versiondir = version
                root = self.modules[module]
            # Compare the version string with the revision ID regexp
            revId = re.match("^r([0-9]+)$", version)
            src = [self.repository, root]
            if revId or version.lower() in ["head", "trunk"]: # a revId or trunk (or head) are checked out from trunk
                src.append("trunk")
                if not isProject:
                    src.append(module)
            else:
                src += ["tags", module, versiondir]
            if isProject: # for projects we check-out only the cmt directory
                src.append("cmt")
            src = "/".join(src)
            if revId: # Special syntax for revision IDs
                src += "@" + revId.group(1)
            dst = module
            if vers_dir:
                dst = os.path.join(dst, versiondir)
            if isProject: # for projects we check-out only the cmt directory
                dst = os.path.join(dst, "cmt")
        return src, dst

    def checkout(self, module, version = "head", dest = None, vers_dir = False, project = False):
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

        src, dst = self._computePaths(module, version, project, vers_dir)
        dst = os.path.join(dest, dst)

        if os.path.isdir(os.path.join(dst,".svn")): # looks like a SVN working copy
            # try with "switch"
            sub_cmd = "switch"
        else:
            # normal case
            sub_cmd = "checkout"

        _svn(sub_cmd, src, dst, stdout = None, stderr = None)

        if not vers_dir and not project:
            # create version.cmt file
            self._create_vers_cmt(os.path.join(dest, module), version)
    
    def tag(self, module, version, isProject = False):
        """
        Create a tag for the given module.
        """
        if self.repositoryVersion < (2,0):
            raise RCSError("tag not implemented for Subversion repositories with "
                           "version < 2.0 (current version %s)" % self.repositoryVersion)
        self._assertModule(module, isProject)
        trunkUrl, _ = self._computePaths(module, "trunk", isProject)
        versionUrl, _ = self._computePaths(module, version, isProject)
        _, _, retcode = _svn("copy",
                             "-m", "Tagging %s %s as %s" % ({True: "project",
                                                             False: "package"}[isProject],
                                                            module, version),
                             trunkUrl, versionUrl, stdout = None, stderr = None)
        return retcode
    
    
    def _getRequirements(self, module, version = "head"):
        """
        Return the content of the trunk/HEAD version of the requirements/project.cmt
        file of the module for the requested version (in the repository). 
        """
        self._assertModule(module, isProject = False)
        url, _ = self._computePaths(module, version, isProject = False)
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
