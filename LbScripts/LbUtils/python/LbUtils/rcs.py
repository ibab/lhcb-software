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
    def __init__(self, module, repository):
        self.module = module
        self.repository = repository
    def __str__(self):
        return "Cannot find module '%(module)s' in repository %(repository)r" % self.__dict__

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
    
    def listModules(self, path = None):
        """
        Return a list of strings identifying the modules below the specified
        path (or the root of the repository if not specified).
        """
        return []
    
    def listVersions(self, module):
        """
        Return a list of strings identifying the known versions of a module.
        """
        return []
    
    def checkout(self, module, dest = None):
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
    __module_version_check_file__ = "/cmt/requirements"
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

    def listModules(self, path = None):
        """
        Return a list of strings identifying the modules below the specified
        path (or the root of the repository if not specified).
        """
        # Get the list of modules
        out, err = _cvs("-d", self.repository, "checkout", "-s")
        # split and remove the lines starting with blank
        modules = [ l.split()[0]
                    for l in out.splitlines()
                    if not l.startswith(" ") ]
        # if path is specified, filter the modules to contain only those starting
        # with "path"
        if path:
            # to be sure that it is the hat and not the begin of the name
            if path[-1] != "/":
                path += "/"
            modules = [ m for m in modules if m.startswith(path) ]
        return modules
    
    def listVersions(self, module):
        """
        Return a list of strings identifying the known versions of a module.
        """
        if module not in self.listModules():
            raise RCSUnknownModuleError(module, self.repository)
        # Get the log of the requirements file
        out, err = _cvs("-d", self.repository, "rlog",
                        module + self.__module_version_check_file__)
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
                tags.append(l.strip().split(":")[0])
            else:
                break
        # Filter and sort the tags
        return filterSortVersions(tags)
    
    def checkout(self, module, version = "head", dest = None, vers_dir = False):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used. 
        """
        # this implies a check on the existence of the module
        versions = self.listVersions(module)
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
        if version.lower() != "head":
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

    def _clearModuleCache(self):
        """
        Clear the cache of module names and versions.
        """
        self._modules = {}
        
    def _ls(self, path):
        """
        List the content of a directory in the repository.
        """
        out, err = _svn("ls", "/".join([self.repository, path]))
        # @TODO add error-checking
        return out.splitlines()

    def _getModulesFromProp(self):
        """
        Return the content of the modules property of the pacakges directory as a list.
        """
        out, err = _svn("propget", "modules", "/".join([self.repository, "packages"]))
        return [ m for m in out.splitlines() if m ]
        
    def listModules(self, path = None, forceScan = False):
        """
        Return a list of strings identifying the modules below the specified
        path (or the root of the repository if not specified).
        """
        if not self._modules:
            # Get the list of modules
            if not forceScan:
                # try using the metadata of the directory "pacakges"                
                modules = self._getModulesFromProp()
            else:
                # initialize to empty
                modules = []
            if not modules:
                # if we do not have modules yet (not found or not using metadata)
                # full scan of the repository (inefficient)
                dirs = [ l for l in self._ls("packages") if l.endswith("/") ]
                while dirs:
                    d = dirs.pop(0)
                    subdirs = [ l for l in self._ls("packages/" + d) if l.endswith("/") ]
                    if "trunk/" in subdirs:
                        modules.append(d[:-1])
                    else:
                        for sd in subdirs:
                            dirs.append(d+sd)
            for m in modules:
                self._modules[m] = None
        if path:
            # to be sure that it is the hat and not the begin of the name
            if path[-1] != "/":
                path += "/"
            return [ m for m in self._modules if m.startswith(path) ]
        return self._modules.keys()
    
    def listVersions(self, module):
        """
        Return a list of strings identifying the known versions of a module.
        """
        if module not in self.listModules():
            raise RCSUnknownModuleError(module, self.repository)
        if self._modules[module] is None:
            tags = [ l[:-1]
                     for l in self._ls("packages/%s/tags" % module)
                     if l.endswith("/") ]
            self._modules[module] = filterSortVersions(tags)
        return self._modules[module]
    
    def checkout(self, module, version = "head", dest = None, vers_dir = False):
        """
        Extract a module in the directory specified with "dest".
        If no destination is specified, the current directory is used. 
        """
        # this implies a check on the existence of the module
        versions = self.listVersions(module)
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
            src = "/".join([self.repository, "packages", module, "tags", version])
        else:
            src = "/".join([self.repository, "packages", module, "trunk"])
        
        dst = os.path.join(dest, module)
        if vers_dir:
            dst = os.path.join(dst, version)
        
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
