"""
Module providing the basic functionalities of the GetPack utility.
"""
import os, re
from subprocess import Popen, PIPE, STDOUT
import logging

_call_command_log = logging.getLogger("_call_command")
def _call_command(cmd, *args, **kwargs):
    """
    Simple wrapper to execute a command and return standard output and standard error.
    """
    d = {"stdout": PIPE, "stderr": PIPE}
    d.update(kwargs)
    cmd = [cmd] + list(args)
    _call_command_log.debug("Execute command: %r %r", " ".join(cmd), kwargs)
    proc = apply(Popen, (cmd,), d)
    return proc.communicate()

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

def filterSortVersions(tags):
    lhcb_style_version = re.compile(r'v([0-9]+)r([0-9]+)(?:p([0-9]+))?')
    versions = []
    for t in tags:
        m = lhcb_style_version.match(t)
        if m:
            # groups gives something like ("1","2","3") or ("1","2",None)
            # and we need to convert it to numbers to sort them 
            version = tuple([ int(n)
                              for n in m.groups()
                              if n ])
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
    
    def _retrieveVersions(self, module, isProject):
        """
        Extract from the repository the list of symbolic names for the module.
        """
        return []
        
    def listVersions(self, module, isProject = False):
        """
        Return a list of strings identifying the known versions of a package.
        """
        if (isProject and module not in self.projects) or \
           ((not isProject) and module not in self.packages):
            raise RCSUnknownModuleError(module, self.repository, isProject)
        return filterSortVersions(self._retrieveVersions(module, isProject))
    
    def checkout(self, module, dest = None, vers_dir = False, project = False):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used. 
        """
        pass

_cvs = lambda *args, **kwargs: apply(_call_command, ("cvs",) + args, kwargs)
class CVS(RevisionControlSystem):
    """
    CVS implementation of RevisionControlSystem.
    """
    __repository_rexp__ = re.compile(r":([pk]server|ext):(?:([\w.]+)@)?([\w.]+):(/[\w./]+)*")
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
        out, err = _cvs("-d", self.repository, "checkout", "-s")
        modules = [ l.split()[0]
                    for l in out.splitlines()
                    if not l.startswith(" ") ]
        packages = []
        projects = []
        # empiric definition:
        # a project is a module all capital letters, with no hat
        proj_re = re.compile("^[A-Z]*$") 
        for m in modules:
            if proj_re.match(m) and m != "CVSROOT":
                projects.append(m)
            else:
                packages.append(m)
        self._packages = packages
        self._projects = projects
    
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
        file_to_check = self.__package_version_check_file__
        if isProject:
            file_to_check = self.__project_version_check_file__
        out, err = _cvs("-d", self.repository, "rlog", module + file_to_check)
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
        # this implies a check on the existence of the module
        versions = self.listVersions(module, project)
        # check for the validity of the version
        if version.lower() != "head" and version not in versions:
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
            options += ("-d", os.path.join(module, version))
        options += (module,)
        apply(_cvs, options, {"cwd": dest, "stdout": None, "stderr": None})
        
        if not vers_dir:
            # create version.cmt file
            open(os.path.join(dest, module, "cmt", "version.cmt"), "w").write(version + "\n")

_svn = lambda *args, **kwargs: apply(_call_command, ("svn",) + args, kwargs)
class SubversionCmd(RevisionControlSystem):
    """
    CVS implementation of RevisionControlSystem.
    """
    __repository_rexp__ = re.compile(r"(svn(?:\+ssh)?|https|file)://(?:([\w.]+)@)?([\w.]*)(/[\w./]+)*")
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
        self._modules = {}
        self._packages = None
    
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
                ( m.group(cls.SVN_PROTOCOL) == "file" and not m.group(cls.SVN_HOST) ) or \
                ( m.group(cls.SVN_PROTOCOL) != "file" and m.group(cls.SVN_HOST) ) 
        return valid
        
    def _ls(self, path):
        """
        List the content of a directory in the repository.
        """
        out, err = _svn("ls", "/".join([self.repository, path]))
        # @TODO add error-checking
        return out.splitlines()

    def _getModulesFromProp(self):
        """
        Return the content of the modules property of the 'packages' directory as a list.
        """
        out, err = _svn("propget", "modules", "/".join([self.repository, "packages"]))
        return [ m for m in out.splitlines() if m ]
    
    def _scanModules(self):
        """
        Scan the directory structure under 'packages' to find modules.
        """
        # full scan of the repository (inefficient)
        modules = []
        dirs = [ l for l in self._ls("packages") if l.endswith("/") ]
        while dirs:
            d = dirs.pop(0)
            subdirs = [ l for l in self._ls("packages/" + d) if l.endswith("/") ]
            if "trunk/" in subdirs:
                modules.append(d[:-1])
            else:
                for sd in subdirs:
                    dirs.append(d+sd)
        return modules
    
    def _retrievePackages(self):
        # if _getModulesFromProp is empty, try _scanModules
        return self._getModulesFromProp() or self._scanModules()
    
    def _retrieveProjects(self):
        # if _getModulesFromProp is empty, try _scanModules
        return [ l[:-1] for l in self._ls("projects") if l.endswith("/") ]
    
    def _topLevel(self, isProject = False):
        if isProject:
            return "projects"
        else:
            return "packages"
        
    def _retrieveVersions(self, module, isProject):
        """
        Extract from the repository the list of symbolic names for the module.
        """
        return [ l[:-1]
                 for l in self._ls("%s/%s/tags" % (self._topLevel(isProject), module))
                 if l.endswith("/") ]
    
    def checkout(self, module, version = "head", dest = None, vers_dir = False, project = False):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used. 
        """
        # this implies a check on the existence of the module
        versions = self.listVersions(module, project)
        # check for the validity of the version
        if version.lower() != "head" and version not in versions:
            raise RCSUnknownVersionError(module, version)
        
        if dest: # prepare destination directory
            if not os.path.isdir(dest):
                os.makedirs(dest, mode = 0755)
            os.chdir(dest)
        else:
            dest = "." # If not specified, use local directory
        
        if version.lower() != "head":
            src = "/".join([self.repository, self._topLevel(project), module, "tags", version])
        else:
            src = "/".join([self.repository, self._topLevel(project), module, "trunk"])
        
        if project:
            version = "%s_%s" % (module, version)
        
        dst = os.path.join(dest, module)
        if vers_dir:
            dst = os.path.join(dst, version)
        if project: # projects in SVN _are_ the content of the "cmt" directory
            dst = os.path.join(dst, "cmt")
        
        _svn("checkout", src, dst, stdout = None, stderr = None)
        
        if not vers_dir:
            # create version.cmt file
            open(os.path.join(dst, "cmt", "version.cmt"), "w").write(version + "\n")


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
