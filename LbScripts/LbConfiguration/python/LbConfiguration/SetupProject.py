#!/usr/bin/env python
# -*- coding: utf-8 -*-
# pylint: disable-msg=E1103,W0141
import os, sys, re, time
from xml.sax import parse, ContentHandler
from stat import S_ISDIR
from fnmatch import fnmatch
from tempfile import mkstemp

from LbConfiguration import createProjectMakefile, createEclipseConfiguration
from LbUtils.Temporary import TempDir
import LbUtils.Path
# Special version of isCVMFS with override
isCVMFS = lambda d: ("SP_FAKE_CVMFS" in os.environ) or LbUtils.Path.isCVMFS(d)

from LbConfiguration.Version import ParseSvnVersion
__version__ = ParseSvnVersion("$Id$", "$URL$")

try:
    from LbScriptsPolicy.PathStripper import StripPath
except ImportError:
    # Failover solution
    StripPath = lambda x: x

# subprocess is available since Python 2.4, but LbUtils guarantees that we can
# import it also in Python 2.3
from subprocess import Popen, PIPE, STDOUT

import logging
# Import all levels constants for consistency with VERBOSE and ALWAYS defined below
from logging import INFO, DEBUG, WARNING, ERROR, FATAL
# Extend the standard logging with one level that sits in the middle between INFO and DEBUG
VERBOSE = (INFO + DEBUG)/2
logging.addLevelName(VERBOSE, "VERBOSE")
ALWAYS = FATAL + 10
logging.addLevelName(ALWAYS, "ALWAYS")
## Specialization of the default Python Logger class to use two more levels (a la LHCb)
class ExtendedLogger(logging.Logger):
    def verbose(self, msg, *args, **kwargs):
        """
        Cloned from the default member functions in logging.Logger.
        """
        if self.isEnabledFor(VERBOSE):
            self._log(VERBOSE, msg, args, **kwargs)
    def always(self, msg, *args, **kwargs):
        """
        Cloned from the default member functions in logging.Logger.
        """
        if self.isEnabledFor(ALWAYS):
            self._log(ALWAYS, msg, args, **kwargs)
logging.setLoggerClass(ExtendedLogger)
log = logging.getLogger(os.path.basename(__file__))

########################################################################
# Useful constants
########################################################################
lhcb_style_version = re.compile(r'v([0-9]+)r([0-9]+)(?:p([0-9]+))?')
lcg_style_version = re.compile(r'([0-9]+)([a-zA-Z_0-9]*)$')
# LHCb standard projects
import LbConfiguration.Project
# LHCb projects without container
import LbConfiguration.Package
# LCGCMT is not an LHCb project but we know about it
nocontainer_project_names = LbConfiguration.Package.project_names + ["LCGCMT", "ROOT"]
project_names = LbConfiguration.Project.project_names + nocontainer_project_names # Add the projects without container to the full list

# List of pairs (project,[packages]) to automatically select for override
# The project are prepended to the list of overriding packages and
# the packages are appended to the list of used packages
auto_override_projects = [("ExtraPackages", []),
                          ("Compat", ["CompatSys"]),
                          #("LHCbGrid", ["LHCbGridSys"]), # enabled with --use-grid
                          ]

# List of abbreviations for weekdays
days = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]

########################################################################
# Utility classes
########################################################################
if sys.platform.startswith("win"):
    _fixKey = lambda key: key.upper()
else:
    _fixKey = lambda key: key

class TemporaryEnvironment:
    """
    Class to changes the environment temporarily.
    """
    def __init__(self, orig = os.environ, keep_same = False):
        """
        Create a temporary environment on top of the one specified
        (it can be another TemporaryEnvironment instance).
        """
        #print "New environment"
        self.old_values = {}
        self.env = orig
        self._keep_same = keep_same

    def __setitem__(self,key,value):
        """
        Set an environment variable recording the previous value.
        """
        key = _fixKey(key)
        if key not in self.old_values :
            if key in self.env :
                if not self._keep_same or self.env[key] != value:
                    self.old_values[key] = self.env[key]
            else:
                self.old_values[key] = None
        self.env[key] = value

    def __getitem__(self,key):
        """
        Get an environment variable.
        Needed to provide the same interface as os.environ.
        """
        key = _fixKey(key)
        return self.env[key]

    def __delitem__(self,key):
        """
        Unset an environment variable.
        Needed to provide the same interface as os.environ.
        """
        key = _fixKey(key)
        if key not in self.env :
            raise KeyError(key)
        # Record that we unset a variable only if it was not set before
        if (key in self.old_values) and (self.old_values[key] is None):
            del self.old_values[key]
        else:
            self.old_values[key] = self.env[key]
        del self.env[key]

    def __len__(self):
        """
        Return the size of the internal dictionary.
        Needed for the conversion to boolean.
        """
        return len(self.env)

    def keys(self):
        """
        Return the list of defined environment variables.
        Needed to provide the same interface as os.environ.
        """
        return self.env.keys()

    def has_key(self,key):
        """
        return True if the key is present
        """
        key = _fixKey(key)
        return (key in self.env.keys())

    def items(self):
        """
        Return the list of (name,value) pairs for the defined environment variables.
        Needed to provide the same interface as os.environ.
        """
        return self.env.items()

    def __contains__(self,key):
        """
        Operator 'in'.
        Needed to provide the same interface as os.environ.
        """
        key = _fixKey(key)
        return key in self.env

    def restore(self):
        """
        Revert all the changes done to the original environment.
        """
        for key,value in self.old_values.items():
            if value is None:
                del self.env[key]
            else:
                self.env[key] = value
        self.old_values = {}

    def __del__(self):
        """
        Revert the changes on destruction.
        """
        #print "Restoring the environment"
        self.restore()

    def get(self, key, default = None):
        """
        Implementation of the standard get method of a dictionary: return the
        value associated to "key" if present, otherwise return the default.
        """
        key = _fixKey(key)
        return self.env.get(key,default)

    def commit(self):
        """
        Forget the old values for the changes done so far (avoids that the
        changes are rolled-back when the instance goes out of scope).
        """
        self.old_values = {}

    def gen_script(self,shell_type):
        """
        Generate a shell script to reproduce the changes in the environment.
        """
        var_re = re.compile(r'(?:\$(\w+|\{[^}]*\})|%(\w+)%)')
        shells = [ 'csh', 'sh', 'bat' ]
        if shell_type not in shells:
            raise RuntimeError("Shell type '%s' unknown. Available: %s" % (shell_type, shells))
        out = ""
        old_vars = self.old_values.keys()
        done_vars = []
        while old_vars:
            key = old_vars.pop(0)
            if key not in self.env:
                # unset variable
                if shell_type == 'csh':
                    out += 'unsetenv %s\n' % key
                elif shell_type == 'sh':
                    out += 'unset %s\n' % key
                elif shell_type == 'bat':
                    out += 'set %s=\n' % key
                done_vars.append(key)
            else:
                val = self.env[key]
                can_be_added = True
                if '$' in val or '%' in val:
                    ms = var_re.findall(val)
                    for group1, group2 in ms:
                        ref_var = group1 or group2
                        if ref_var.startswith('{') and ref_var.endswith('}'):
                            ref_var = ref_var[1:-1]
                        if ref_var not in done_vars and ref_var in old_vars:
                            can_be_added = False
                            break
                if can_be_added:
                    # set variable
                    if shell_type == 'csh':
                        out += 'setenv %s "%s"\n' % (key, self.env[key])
                    elif shell_type == 'sh':
                        out += 'export %s="%s"\n' % (key, self.env[key])
                    elif shell_type == 'bat':
                        out += 'set %s=%s\n' % (key, self.env[key])
                    done_vars.append(key)
                else:
                    old_vars.append(key)
        return out
    def __iter__(self):
        """
        Forward the iteration to the internal dictionary.
        """
        return self.env.__iter__()

def _sync_dicts(src, dest):
    # remove undefined keys
    keys = dest.keys()
    for k in keys:
        if k not in src:
            del dest[k]
    # set the variables
    for k in src:
        dest[k] = src[k]

def isProject(path, ignore_not_ready = False, cvmfs = False):
    # It is a project directory if it contains cmt/project.cmt and it doesn't
    # contain NOT_READY (unless we are told to ignore that flag).
    # NOT_READY -> projects that are being released
    if cvmfs:
        # On CVMFS it's too heavy to look for the project.cmt file, so we rely on
        # the naming convention (path/to/PROJECT/PROJECT_vXrY).
        try:
            # take the last two elements of the path
            p, v = path.rsplit(os.path.sep, 2)[-2:]
            # check that they are like PROJECT and PROJECT_vXrY
            if v.startswith(p + "_"):
                v = v[len(p)+1:]
                return lhcb_style_version.match(v) or lcg_style_version.match(v)
            else:
                # in case of projects without version, 'v' is the name of the project
                return v in LbConfiguration.Package.project_names
        except:
            return False
    # on other filesystems we use a better check
    return os.path.isfile(os.path.join(path,'cmt','project.cmt')) and \
           (ignore_not_ready
            or not os.path.exists(os.path.join(path, 'NOT_READY')))

def _extract_version(project, version):
    # By default, the version is the name of the version directory
    v = version
    # if the version directory starts with the project name, we use the
    # exceeding part (GAUDI_v20r0 -> v20r0)
    if version.startswith(project):
        if len(version) > len(project): # the version directory could be == project
            v = version[len(project):].lstrip("_")
            if not v: # handle the case "Gaudi_" considering '_' as version
                v = version[len(project):]
    return v

def FindProjectVersions(project, search_path, user_area = None,
                        ignore_not_ready = False):
    """Given a project name, discovers all the matching project/versions using
    the provided user_area and the the search_path (a list of directories).
    Returns a list of tuples with (name, version, realname, basepath) where
      'name'     is the canonical name of the project (e.g. Gaudi, DaVinci)
      'version'  is the canonical version string (e.g. v2r3)
      'realname' is the name of the root directory of the project (e.g GAUDI/GAUDI_v21r6,
                 LHCb_v28r3, DBASE)
      'basepath' is the directory in which 'realname' is based.
    If there is no version the second entry is None.
    """
    versions = [] # container of the results
    # Look into user_area for projects (without version directory)
    if user_area and os.path.isdir(user_area):
        user_projects = [d for d in os.listdir(user_area)
                         if isProject(os.path.join(user_area,d),
                                      ignore_not_ready, False)] # User area never on CVMFS
        # look for projects with names starting with <project>
        candidates = [d for d in user_projects
                      if d.startswith(project)]
        # If there is no user project with the given name, try to use the
        # correct case for the project
        if not candidates:
            prj = FixProjectCase(project)
            candidates = [d for d in user_projects
                          if d.startswith(prj)]
        # extract 'fake' version informations
        for c in candidates:
            if c == project:
                v = None
            else:
                v = _extract_version(project,c)
            versions.append((project, v, c, user_area))

    candidates = []
    # find all project directories in the searchpath
    PROJECT = project.upper()
    # search_path = os.environ["CMTPROJECTPATH"].split(os.path.pathsep)
    for d in search_path:
        cvmfs = isCVMFS(d) # let's assume that we do not have mount points inside a CVMFS
        p = os.path.join(d,PROJECT)
        if os.path.isdir(p):
            if isProject(p, ignore_not_ready, cvmfs):
                # project without version
                versions.append((project, None, PROJECT, d))
            else:
                # project with versions
                versions += [ (project, _extract_version(PROJECT,v),
                               os.path.join(PROJECT,v), d)
                               for v in os.listdir(p)
                               if isProject(os.path.join(p,v), ignore_not_ready, cvmfs) ]
    return versions

def FindLCGForROOT(version, versions):
    '''
    Return the version of LCGCMT for the required version of ROOT.

    @param versions: list of tuples like the one returned by FindProjectVersions
    '''
    from os.path import join, exists
    root_version = re.compile(r'^\s*macro\s+ROOT_config_version\s+"([^"]*)"')
    for req, vers in [(join(base, name,
                           'LCG_Configuration', 'cmt', 'requirements'),
                       vers)
                      for _, vers, name, base in versions]:
        if exists(req):
            for l in open(req):
                m = root_version.match(l)
                if m:
                    if m.group(1) == version:
                        return vers # got it
                    else:
                        break # we found the declaration of ROOT, but we do not like it
    return None

def SortVersions(versions, reverse = False):
    """Give a list of version numbers, return a list with only unique elements
    ordered by version.
    """
    # remove duplicates
    tmp_list = uniq(versions)
    sortable_list = []
    for v in tmp_list:
        if v is None:
            sortable_list.append( ((0,0,v), v) )
        else:
            m = lhcb_style_version.match(v)
            if m:
                a,b,c = m.groups()
                a = int(a)
                b = int(b)
                if c: c = int(c)
                else: c = 0
                sortable_list.append( ((a,b,c), v) )
            else:
                m = lcg_style_version.match(v)
                if m:
                    a,b = m.groups()
                    a = int(a)
                    sortable_list.append( ((0,a,b), v) )
                elif v == "HEAD":
                    sortable_list.append( ((10000,0,0), v) )
                else:
                    sortable_list.append( ((0,0,v), v) )
    ## @FIXME: in Python 2.3 the "reverse" argument is not available
    # sortable_list.sort(reverse = reverse)
    sortable_list.sort()
    if reverse: sortable_list.reverse()
    return [ v[1] for v in sortable_list ]

def SortVersionTuples(versions, reverse = False):
    '''
    Similar to SortVersions, but take the list of versions returned by FindVersions.
    '''
    vers_map = dict([(t[1], t) for t in versions[::-1]])
    return [vers_map[k] for k in SortVersions(vers_map, reverse)]

def LatestVersion(versions):
    """Extract the latest version from a list of version tuples
    (output of FindProjectVersions)."""
    return SortVersions([ v[1] for v in versions ])[-1]

def VersionMatch(version, pattern):
    """Compare a version string with a pattern. The pattern can be
    the exact match, a glob pattern or None (in which case matches an None version).
    """
    return version == pattern or \
        ((pattern is not None) and (version is not None) and fnmatch(version, pattern))

def _GetVersionTuple(pattern, versions):
    """Extract the version tuple corresponding to version 'pattern'.
    """
    if not versions:
        # No need to go on if the versions list is empty
        return None
    # Extract the list of version strings (in the correct order)
    version_strings = SortVersions([v[1] for v in versions], reverse = True)
    # Look for a match in the list of versions
    found = False
    match = None
    for v in version_strings:
        if VersionMatch(v, pattern): # compare the version with the pattern
            found = True
            match = v
            break # exit the loop at the first match
    if not found and pattern is None: # fall back solution
        # Normal behavior
        match = version_strings[0] # latest version

    # Now that we have a string (and not a pattern), we can extract the tuple
    for vers_tuple in versions:
        if match == vers_tuple[1]:
            return vers_tuple
    # Nothing found
    return None

class ProjectInfo:
    """
    Class to get project related infos.
    """
    # @todo: use LHCB_config.py instead of scanning directories
    searchpath = []
    # Projects for which the check for the platform does not make sense.
    no_platform_projects = ['Compat', 'LbScripts', 'LCGCMT', 'ExtraPackages', 'LHCbDirac', 'LHCbGrid'] + nocontainer_project_names

    def __init__(self, project, version, realName, path):
        self.name = FixProjectCase(project)
        self.version = version
        self.realName = realName
        self.path = path
        self.project_dir = os.path.join(self.path, self.realName)

        # discover project policy
        if self.version:
            self._projectenv_cmt_dir = os.path.join(self.path,"%sEnv"%self.name,self.version,'cmt')
            if os.path.isdir(self._projectenv_cmt_dir):
                self.policy = 'old'
            else:
                self._projectenv_cmt_dir = None
                self.policy = 'new'
        else:
            # Projects without version must be considered "new" style
            self._projectenv_cmt_dir = None
            self.policy = 'new'

        self.sys = self._container()
        if self.version and self.sys:
            self.syscmtfullpath = os.path.join(self.project_dir,self.sys,self.version,'cmt')
        else:
            self.syscmtfullpath = None

    def _container(self):
        """
        Return the name of the package referencing all the packages belonging to a project.
        """
        container = None
        project_cmt = os.path.join(self.path, self.realName, "cmt", "project.cmt")
        if os.path.exists(project_cmt):
            for l in open(project_cmt):
                tokens = l.split()
                if tokens and tokens[0] == "container":
                    container = tokens[1]
                    break
        if container is None:
            if self.name == 'Gaudi':
                if self.policy == 'new':
                    container = self.name + "Release"
                else:
                    container = self.name
            elif self.name in nocontainer_project_names: # Project without container
                container = None
            else:
                container = self.name + 'Sys'
        return container

    def __str__(self):
        if self.version:
            return "%s %s from %s" % (self.name, self.version, self.project_dir)
        else:
            return "%s from %s" % (self.name, self.project_dir)

    def supportsPlatform(self, platform, user_area=None):
        """
        Check if the project supports the requested platform (i.e. it contains
        the correct platform directory in the InstallArea).
        """
        return (self.path == user_area
                or self.name in self.no_platform_projects
                or os.path.isdir(os.path.join(self.project_dir, 'InstallArea', platform)))


def _defaultSearchPath(env = None):
    search_path = []
    if env is None:
        env = os.environ
    for v in ["CMTPROJECTPATH", "LHCBPROJECTPATH"]:
        if v in env:
            search_path += env[v].split(os.pathsep)
    return search_path

def makeProjectInfo(project = None, version = None, versions = None, search_path = None, user_area = None,
                    env = None, ignore_not_ready = False):
    # actual body
    if versions is None:
        if not project:
            raise TypeError("makeProjectInfo() requires either 'project' or 'versions'")
        if not search_path: # default search path
            search_path = _defaultSearchPath(env)
            #raise TypeError("makeProjectInfo() requires 'search_path' if 'versions' is not specified")
        versions = FindProjectVersions(project, search_path, user_area, ignore_not_ready)
    vers_tuple = _GetVersionTuple(version, versions)
    if not vers_tuple:
        return None
    return apply(ProjectInfo,vers_tuple)

if sys.platform.startswith("win"):
    # On Windows, Popen needs a list (when using shell=True)
    _prepare_cmt_cmd = lambda cmd, args: ["cmt", cmd] + args
else:
    # on Unix, we need a string
    _prepare_cmt_cmd = lambda cmd, args: \
                        " ".join(["cmt", cmd] + map(lambda s: '"%s"' % s, args))

def cmt(command, args = [], environment = None, cwd = None):
    if type(args) is str:
        args = [args]
    cmd = _prepare_cmt_cmd(command, args)
    if not cwd:
        cwd = os.getcwd()
    if not environment:
        environment = os.environ
    return Popen(cmd, shell = True,
                 env = environment, cwd = cwd,
                 stdout = PIPE, stderr = STDOUT).communicate()[0]

class GetNightlyCMTPROJECTPATH(ContentHandler):
    """SAX content handler to extract the CMTPROJECTPATH from lcg nightly build
    configuration for a given slot/day.
    Usage:
    from xml.sax import parse
    getter = GetNightlyCMTPROJECTPATH("lhcb2","Mon")
    parse(filename, getter)
    print getter.value()
    """
    def __init__(self, slot, day = None):
        ContentHandler.__init__(self)
        self.slot = slot
        self._path = []
        self._in_slot = False
        self._in_cmtpp = False
        if day in (None, 'Today'):
            self._day = time.localtime()[6] # today
        elif day in days:
            self._day = days.index(day)
        elif day in range(7):
            self._day = day
        else:
            raise TypeError("What do you mean with day = %r?" % day)
    def startElement(self, name, attrs):
        """SAX implementation.
        """
        if name == "slot":
            if attrs["name"] == self.slot:
                self._in_slot = True
        elif name == "cmtprojectpath":
            self._in_cmtpp = True
        elif name == "path":
            if self._in_slot  and self._in_cmtpp:
                self._path.append(attrs["value"])
    def endElement(self, name):
        """SAX implementation.
        """
        if name == "slot":
            self._in_slot = False
        elif name == "cmtprojectpath":
            self._in_cmtpp = False
    def dirs(self):
        """Return the list of entries in the CMTPROJECTPATH.
        """
        today = days[self._day]
        yesterday = days[self._day-1]
        d = [ s.replace("%DAY%",today).replace("%YESTERDAY%",yesterday)
              for s in self._path ]
        return d
    def value(self):
        """Return the CMTPROJECTPATH.
        """
        return os.path.pathsep.join(self.dirs())

def getNightlyCMTPROJECTPATH(path, slot, day):
    """Simple wrapper around GetNightlyCMTPROJECTPATH.
    Returns the list of entries in the CMTPROJECTPATH of the slot of the nightly
    build.
    """
    getter = GetNightlyCMTPROJECTPATH(slot, day)
    parse(path, getter)
    return getter.dirs()


def smartExpandVarsPath(path, env = None):
    """
    Expand the environment variables in a path. If the variable in the entry of
    the path is a path itself, one entry is added for each element of the
    variable. (The code is more or less inspired by posixpath.py)
    Note: replaces only the first variable in each entry.
    """
    if env is None:
        env = os.environ
    entries = path.split(os.pathsep)
    new_entries = []
    # matches:
    #   $varname
    #   ${varname}
    #   %varname%
    var_re = re.compile(r'(?:\$(\w+|\{[^}]*\})|%(\w+)%)')
    for e in entries:
        if '$' not in e and '%' not in e:
            new_entries.append(e)
        else:
            match = var_re.search(e)
            if match:
                name = match.group(1) or match.group(2)
                if name.startswith('{') and name.endswith('}'):
                    name = name[1:-1]
                if name not in env:
                    new_entries.append(e)
                else:
                    a, b = match.span(0)
                    ea, eb = e[:a], e[b:]
                    for ve in env[name].split(os.pathsep):
                        new_entries.append(ea+ve+eb)
    return os.pathsep.join(new_entries)

########################################################################
# Utility functions
########################################################################

def uniq(iterable):
    """Returns a list of unique elements in the passed iterable.
    """
    result = []
    for i in iterable:
        if i not in result:
            result.append(i)
    return result

def removeall(path):
    """
    Recursively remove directories and files.
    """
    log.debug("----- removeall(%s) -----", path)
    if S_ISDIR(os.stat(path).st_mode):
        lst = os.listdir(path)
        for p in lst:
            removeall(os.path.join(path,p))
        os.rmdir(path)
    else:
        os.remove(path)

def _get_cmt_user_context(env = None):
    log.debug("----- get_cmt_user_context() -----")
    if env is None:
        env = os.environ
    if 'CMTPATH' not in env:
        # This function needs CMTPATH, which means that it does not work with CMTPROJECTPATH
        log.debug("----- get_cmt_user_context(): no CMTPATH -----")
        return None # For the moment we ignore the CMTUSERCONTEXT

    # check if we can use CMTUSERCONTEXT
    context_path = None
    context_validity = None
    current_lcgcmt = None

    # Find the context validity
    if env['LHCb_release_area']:
        context_path = os.path.join(env['LHCb_release_area'],'context')
        context_file = os.path.join(context_path,'requirements')

        if os.access(context_file,os.F_OK | os.R_OK):
            # find the magic comment
            for l in open(context_file).xreadlines():
                m = re.match("^#.*created.*from.*LCGCMT_(([0-9]+)([a-z]?)|HEAD)", l)
                if m :
                    context_validity = m.groups()
                    break

    if not context_validity:
        log.debug("----- get_cmt_user_context(): context not found -----")
        return None
    else:
        log.debug("----- get_cmt_user_context(): context if from LCGCMT %s -----", context_validity[0])

    # we have to compare the version with the current one
    for l in env['CMTPATH'].split(os.pathsep):
        m = re.match("^.*LCGCMT_(([0-9]+)([a-z]?)|HEAD)", l)
        if m:
            current_lcgcmt = m.groups()
            break
    if current_lcgcmt:
        log.debug("----- get_cmt_user_context(): Default LCGCMT is version %s -----", current_lcgcmt[0])

    if not current_lcgcmt:
        # for no lcgcmt in the path, assume we need the context
        log.debug("----- get_cmt_user_context(): LCGCMT not found, use %s -----", context_path)
        return context_path

    # at this point we have both context and current: comparison time
    if context_validity[0] == current_lcgcmt[0] or current_lcgcmt[0] == 'HEAD':
        # the current is good
        log.debug("----- get_cmt_user_context(): current LCGCMT is good -----")
        return None

    if context_validity[0] == 'HEAD':
        # the current cannot be better
        log.debug("----- get_cmt_user_context(): use %s -----", context_path)
        return context_path

    if int(context_validity[1]) > int(current_lcgcmt[1]):
        # context is better
        log.debug("----- get_cmt_user_context(): use %s -----", context_path)
        return context_path
    elif (context_validity[1] == current_lcgcmt[1]) and (context_validity[2] > current_lcgcmt[2]):
        # like in 46b > 46a.. context is still better
        log.debug("----- get_cmt_user_context(): use %s -----", context_path)
        return context_path
    else:
        # it seems that the current one is better
        log.debug("----- get_cmt_user_context(): do not use the context -----")
        return None

def shParser(script, env):
    leftover = []
    for l in script.splitlines():
        if l.startswith("unset"):
            var = l.split()[-1]
            if var in env:
                del env[var]
        elif "export " in l:
            l = l.split(";")[0]
            var, value = l.split("=",1)
            value = os.path.expandvars(value.strip('"')) # unix style vars
            env[var] = value
        else:
            leftover.append(l)
    if leftover:
        leftover.append("") # an empty line to have the final newline
    return "\n".join(leftover)

def cshParser(script, env):
    leftover = []
    for l in script.splitlines():
        if l.startswith("unsetenv"):
            var = l.split()[-1]
            if var in env:
                del env[var]
        elif l.startswith("setenv"):
            dummy, var, value = l.split(None,2)
            value = os.path.expandvars(value.strip('"')) # unix style vars
            env[var] = value
        else:
            leftover.append(l)
    if leftover:
        leftover.append("") # an empty line to have the final newline
    return "\n".join(leftover)

def batParser(script, env):
    win_var = re.compile(r"%([^0-9][^%]*)%")
    leftover = []
    for l in script.splitlines():
        if l.startswith("set"):
            l = l.split(None,1)[1]
            var, value = l.split("=",1)
            value = os.path.expandvars(value) # unix style vars
            value = win_var.sub(r'%(\1)s',value) % env # win style vars
            env[var] = value
        else:
            leftover.append(l)
    if leftover:
        leftover.append("") # an empty line to have the final newline
    return "\n".join(leftover)

ShellParser = {"sh":shParser,
               "csh":cshParser,
               "bat":batParser}

def FixProjectCase(project):
    """Convert the case of the project name to the correct one, based on a list
    of known project names.
    If the project is not known, the name is returned unchanged.
    """
    proj = project.lower()

    for p in project_names:
        if p.lower() == proj:
            return p
    return project

########################################################################
# Main class
########################################################################
class SetupProjectError(RuntimeError):
    pass
class SetupProject(object):
    def __init__(self):
        self._logger = logging.getLogger("SetupProject")

        ## Dictionary to store the changes to the environment.
        #  It is initialized as a copy of the current environment.
        self.environment = TemporaryEnvironment(dict(os.environ))

        self.opts = None
        self.args = None

        self.project_name = None
        self.project_version = None
        self.project_info = None

        self.context_path = None

        self.output_file = sys.stdout
        self.tmp_root = None

        if not self.check_environment():
            raise RuntimeError("Environment sanity check failed")

        self.default_externals = {
                                  'none':[],
                                  'CERN':[], # ['CASTOR'],
                                 }
        self.parser = self._prepare_parser()
        self.overriding_projects = None
        self.runtime_projects = None
        self.search_path = None
        self.user_area = None

    def __getattr__(self,attr):
        """
        Shortcut to retrieve the options
        """
        if self.opts:
            return getattr(self.opts,attr)
        raise AttributeError("'%s' object has no attribute '%s'"%
                             (self.__class__.__name__,attr))

    ## Alias to forward calls to the internal logger
    def _log(self, level, msg, *args, **kwargs):
        apply(self._logger.log, [level, msg] + list(args), kwargs)
    ## Alias to forward calls to the internal logger
    def _always(self, msg, *args, **kwargs):
        apply(self._logger.always, [msg] + list(args), kwargs)
    ## Alias to forward calls to the internal logger
    def _error(self, msg, *args, **kwargs):
        apply(self._logger.error, [msg] + list(args), kwargs)
    ## Alias to forward calls to the internal logger
    def _warning(self, msg, *args, **kwargs):
        apply(self._logger.warning, [msg] + list(args), kwargs)
    ## Alias to forward calls to the internal logger
    def _info(self, msg, *args, **kwargs):
        apply(self._logger.info, [msg] + list(args), kwargs)
    ## Alias to forward calls to the internal logger
    def _verbose(self, msg, *args, **kwargs):
        apply(self._logger.verbose, [msg] + list(args), kwargs)
    ## Alias to forward calls to the internal logger
    def _debug(self, msg, *args, **kwargs):
        apply(self._logger.debug, [msg] + list(args), kwargs)

    ## Helper function to simplify the calls to CMT.
    def cmt(self, cmd, args = [], cwd = None):
        return cmt(cmd, args, environment = dict(self.environment), cwd = cwd)

    def check_environment(self):
        """
        Check that the current environment is usable.
        """
        self._debug("----- check_environment() -----")
        output = self.cmt("version")
        if output.find('command not found') > 0 or output.find('not recognized') > 0 :
            self._error("I cannot find cmt (it is not in your PATH)")
            return False
        if not 'CMTCONFIG' in self.environment:
            self._error("You do not have CMTCONFIG set")
            return False
        return True

    def set_build_env(self):
        self._debug("----- set_build_env() -----")
        if not self.project_info:
            # do nothing
            # FIXME: is this an error?
            return
        if self.project_info.policy =='old':# old style project
            self._debug("----- old style project -----")
            if 'CMTPROJECTPATH' in self.environment:
                self._debug("----- unsetenv CMTPROJECTPATH -----")
                del self.environment['CMTPROJECTPATH']
            # check if we have ExtraPackages in the override_projects
            ep_pi = None
            for pi in self.overriding_projects:
                if pi.name == "ExtraPackages":
                    ep_pi = pi
                    break
            if ep_pi:
                # if ExtraPackages is there, I prepend it to the CMTPATH
                if 'CMTPATH' in self.environment:
                    self.environment['CMTPATH'] = os.pathsep.join([self.environment['CMTPATH'],
                                                                   ep_pi.project_dir])
                else:
                    self.environment['CMTPATH'] = ep_pi.project_dir
            # get all the variables from <Project>Env
            localEnv = dict(self.environment)
            ShellParser[self.shell](self.cmt("setup", "-" + self.shell,
                                             self.project_info._projectenv_cmt_dir),
                                    localEnv)
            # get the CMTPATH from <Project>Env without variable expansion
            cmtpath = os.popen("cmt show set CMTPATH").readlines()[-1].strip()
            if cmtpath.startswith("CMTPATH="): # remove head of the line
                cmtpath = cmtpath[8:]
            cmtpath = cmtpath.strip("'") # remove quotes
            # expand (all) the environment variables and set CMTPATH
            cmtpath_expanded = ""
            while cmtpath_expanded != cmtpath:
                if cmtpath_expanded: cmtpath = cmtpath_expanded
                cmtpath_expanded = smartExpandVarsPath(cmtpath, localEnv)
            self.environment['CMTPATH'] = cmtpath
            # prepend User_release_area if defined
            if self.user_area:
                if self.environment['CMTPATH'].find(self.user_area) < 0:
                    self.environment['CMTPATH'] = os.pathsep.join([self.user_area,
                                                                   self.environment['CMTPATH']])
            self._debug("----- CMTPATH set to '%s' -----", self.environment['CMTPATH'])
        else:
            self.environment['CMTPROJECTPATH'] = os.pathsep.join(self.search_path)
            self._debug("----- CMTPROJECTPATH set to '%s' -----", self.environment['CMTPROJECTPATH'])

            # unset CMTPATH if present
            if 'CMTPATH' in self.environment:
                self._debug("----- unsetenv CMTPATH -----")
                del self.environment['CMTPATH']

    def _write_script(self, data):
        close_output = False
        if self.opts.output:
            if self.append:
                self.output_file = open(self.opts.output,"a")
            else:
                self.output_file = open(self.opts.output,"w")
            self.opts.output = None # reset the option value to avoid to reuse it
            close_output = True
        elif self.opts.mktemp:
            fd, outname = mkstemp()
            self.output_file = os.fdopen(fd,"w")
            print outname
            self.opts.mktemp = None # reset the option value to avoid to reuse it
            close_output = True
        # write the data
        self.output_file.write(data)
        self.output_file.write("\n") # @TODO: this may be avoided
        if close_output: self.output_file.close()

    def _prepare_parser(self):
        from optparse import OptionParser, \
                             IndentedHelpFormatter as BasicHelpFormatter,\
                             OptionValueError

        class MyHelpFormatter(BasicHelpFormatter):
            def format_option(self,option):
                if option.help and option.help.find("(internal)") != -1:
                    return ""
                return BasicHelpFormatter.format_option(self,option)

            def format_option_strings (self, option):
                """Return a comma-separated list of option strings & metavariables."""
                if option.takes_value():
                    metavar = option.metavar or option.dest.upper()
                    short_opts = [sopt + " " + metavar for sopt in option._short_opts]
                    long_opts = [lopt + " " + metavar for lopt in option._long_opts]
                else:
                    short_opts = option._short_opts
                    long_opts = option._long_opts

                if self.short_first:
                    opts = short_opts + long_opts
                else:
                    opts = long_opts + short_opts

                return ", ".join(opts)

        class MyOptionParser(OptionParser):
            def print_help(self, stream = None):
                if stream is None:
                    stream = sys.stderr
                return OptionParser.print_help(self, stream)
            def print_version(self, stream = None):
                if stream is None:
                    stream = sys.stderr
                return OptionParser.print_version(self, stream)

        parser = MyOptionParser(usage = "%prog [options] <project_name> [version|--ask] [options] [externals]",
                                version = __version__,
                                formatter = MyHelpFormatter())

        def check_output_options_cb(option, opt_str, value, parser):
            if opt_str == "--mktemp":
                if parser.values.output:
                    raise OptionValueError("--mktemp cannot be used at the same time as --output or --append")
                parser.values.mktemp = True
            elif opt_str in ["--output", "--append"] :
                if parser.values.mktemp:
                    raise OptionValueError("--mktemp cannot be used at the same time as --output or --append")
                parser.values.output = value
                parser.values.append = (opt_str == "--append")

        # internal options
        parser.add_option("--shell", action="store", type="choice", metavar="SHELL",
                          choices = ['csh','sh','bat'],
                          help="(internal) select the type of shell to use")
        parser.add_option("--output", action="callback", metavar="FILE",
                          type = "string", callback = check_output_options_cb,
                          help="(internal) output the command to set up the environment ot the given file instead of stdout")
        parser.add_option("--append", action="callback", metavar="FILE",
                          type = "string", callback = check_output_options_cb,
                          help="(internal) same as --output, but do not truncate")
        parser.add_option("--mktemp", action="callback",
                          callback = check_output_options_cb,
                          help="(internal) send the output to a temporary file and print on stdout the file name (like mktemp)")

        def default_externals_cb(option, opt_str, value, parser):
            parser.values.site_externals = self.default_externals[value]

        # public options
        parser.add_option("--site", action="callback", type="choice", metavar="SITE",
                          choices=self.default_externals.keys(),
                          callback=default_externals_cb,
                          help="enable site specific defaults")
        parser.add_option("--ask", action="store_true",
                          help="ask for the version of the project to use (overrides the version specified)")
        parser.add_option("--disable-CASTOR", action="store_true",
                          help="remove CASTOR from the added dependencies")
        parser.add_option("--tag_add", action="append",
                          help="specify extra CMT tags")
        parser.add_option("--use", action="append",
                          help="add a CMT use statement")
        parser.add_option("--verbose", action="store_const",
                          dest="loglevel", const=VERBOSE,
                          help="be a bit more verbose")
        parser.add_option("--debug", action="store_const",
                          dest="loglevel", const=DEBUG,
                          help="output useful for debugging")
        parser.add_option("--ignore-missing", action="store_true",
                          dest = "force",
                          help="obsolete, equivalent to --force")
        parser.add_option("--force", action="store_true",
                          help="ignore warnings and errors from CMT, just complain")
        parser.add_option("--ignore-context", action="store_true",
                          help="do not use CMTUSERCONTEXT even if it should be used")
        parser.add_option("--list-versions", action="store_true",
                          help="print available versions of the specified project and exit (all other options are ignored)")
        parser.add_option("--build-env", action="store_true",
                          help="sets only the build time environment for the project")
        parser.add_option("--external-only", action="store_true",
                          help="sets only the environment for the externals (the project is used only to select the version of LCG)")

        def dev_dir_cb(option, opt_str, value, parser):
            if value is None:
                if "LHCBDEV" in self.environment:
                    value = self.environment["LHCBDEV"]
                else:
                    raise OptionValueError("--dev used, but LHCBDEV is not defined")
            parser.values.dev_dirs.append(value)
        parser.add_option("--dev", action="callback",
                          callback = dev_dir_cb,
                          help="prepend $LHCBDEV to the search path. " +
                               "Note: the directories are searched in the "+
                               "order specified on the command line.")
        parser.add_option("--dev-dir", action="callback", metavar="DEVDIR",
                          type = "string", callback = dev_dir_cb,
                          help="prepend DEVDIR to the search path. " +
                               "Note: the directories are searched in the "+
                               "order specified on the command line.")

        parser.add_option("--user-area", action="store",
                          help="Use the specified path as User_release_area instead of "
                               "the value of the environment variable.")

        def external_version_option(_option, opt_str, value, parser):
            if len(parser.largs) < 2:
                raise OptionValueError("%s must be preceded by the name of the external"%opt_str)
            parser.values.ext_versions[parser.largs.pop()] = value

        parser.add_option("-v", action="callback", metavar="VERSION",
                          type="string", callback=external_version_option,
                          help="must be used after the name of an external to specify a non default version for it")

        parser.add_option("--set-CMTPATH", action="store_true",
                          help="Set CMTPATH to the value used internally by CMT (DANGEROUS)")


        def runtime_project_option(_option, opt_str, _value, parser):
            if len(parser.rargs) < 1:
                raise OptionValueError("%s must be followed by the project name and optionally by the version"%opt_str)
            p_name = parser.rargs.pop(0)
            # allow version strings of the LHCb style, LCGCMT style or glob patterns cadidates
            if len(parser.rargs) and re.match(r"^(v[0-9]+r[0-9]+(p[0-9]+)?)|([0-9]+[a-z]?)|(.*[\[\]\*\?].*)$",parser.rargs[0]):
                v = parser.rargs.pop(0)
            else:
                v = None
            parser.values.runtime_projects.append( (p_name,v) )

        parser.add_option("--runtime-project", action="callback",
                          metavar = "PROJECT [VERSION]", type="string",
                          callback=runtime_project_option,
                          nargs = 0,
                          help="Add a project to the runtime environment")

        def overriding_project_option(_option, opt_str, _value, parser):
            if len(parser.rargs) < 1:
                raise OptionValueError("%s must be followed by the project name and optionally by the version"%opt_str)
            p_name = parser.rargs.pop(0)
            if len(parser.rargs) and re.match(r"^(v[0-9]+r[0-9]+(p[0-9]+)?)|([0-9]+[a-z]?)|(.*[\[\]\*\?].*)$",parser.rargs[0]):
                v = parser.rargs.pop(0)
            else:
                v = None
            parser.values.overriding_projects.append( (p_name,v) )

        parser.add_option("--overriding-project", action="callback",
                          metavar = "PROJECT [VERSION]", type="string",
                          callback=overriding_project_option,
                          nargs = 0,
                          help="Add a project to override packages")

        parser.add_option("--no-auto-override", action="store_false",
                          dest = "auto_override",
                          help = "Do not automatically prepend the projects %s" % auto_override_projects)

        parser.add_option("--use-grid", action="store_true",
                          help = "Enable auto selection of LHCbGrid project")

        parser.add_option("-q", "--quiet", action="store_true",
                          help = "Removes message printout during setup")

        parser.add_option("--silent", action="store_true",
                          help = "Avoid any printout (even errors)")

        parser.add_option("--keep-CMTPROJECTPATH", action="store_true",
                          help = "Do not override the value of the environment variable CMTPROJECTPATH")

        def nightly_option(_option, opt_str, _value, _parser):
            parser.values.dev = True
            if len(parser.rargs) < 1:
                raise OptionValueError("%s must be followed by the slot of the nightlies and optionally by the day"%opt_str)
            slot = parser.rargs.pop(0)
            if len(parser.rargs) and parser.rargs[0].capitalize() in days:
                day = parser.rargs.pop(0).capitalize()
            else:
                day = days[time.localtime()[6]]
                # In Python 2.3
                # day = days[datetime.date.today().weekday()]
            # Locate the requested slot in the know nightlies directories
            nightly_bases = [self.environment.get("LHCBNIGHTLIES", "/afs/cern.ch/lhcb/software/nightlies"),
                             self.environment.get('LCG_nightlies_area', '/afs/cern.ch/sw/lcg/app/nightlies')]
            slot_dir = None
            for nightly_base in nightly_bases:
                slot_dir = os.path.join(nightly_base, slot)
                if os.path.isdir(slot_dir): break # exit from the loop as soon as the slot is found
            if not slot_dir:
                raise OptionValueError("Cannot find slot %s in %s. Check the values of the option %s" % (slot, nightly_bases, opt_str))
            path = os.path.join(slot_dir, day)
            if not os.path.isdir(path):
                raise OptionValueError("The directory %s does not exists. Check the values of the option %s" % (path, opt_str))
            parser.values.dev_dirs.append(path)
            # Get the extra CMTPROJECTPATH entries needed for the nightlies
            extraCMTPROJECTPATH = None
            confSumm_file = os.path.join(path, "confSummary.py")
            config_file = os.path.join(path, "configuration.xml")
            if os.path.exists(confSumm_file): # Try with the python digested version
                data = {}
                exec open(confSumm_file).read() in data #IGNORE:W0122
                # Get the list and convert it to strings
                extraCMTPROJECTPATH = filter(str, data.get("cmtProjectPathList",[]))
            elif os.path.exists(config_file): # Try with the XML configuration
                extraCMTPROJECTPATH = getNightlyCMTPROJECTPATH(config_file, slot, day)
            if extraCMTPROJECTPATH:
                parser.values.dev_dirs += extraCMTPROJECTPATH

        parser.add_option("--nightly", action="callback",
                          metavar = "SLOT [DAY]",  type="string",
                          callback = nightly_option,
                          nargs = 0,
                          help = "Add the required slot of the LHCb nightly " +
                                 "builds to the list of DEV dirs. DAY must be "+
                                 "a 3 digit abbreviation of the weekday, " +
                                 "by default the current day. Special settings "+
                                 "of the CMTPROJECTPATH needed for the "+
                                 "nightly build slot are taken into account.")

        parser.add_option("--no-user-area", action="store_true",
                          help = "Ignore the user release area when looking for projects.")

        parser.add_option("--no-touch-logfile", action="store_false",
                          dest="touch_logfile", default=True,
                          help="Avoid touching the logfiles used to identify active projects.")

        parser.add_option("--ignore-not-ready", action="store_true",
                          help = "Ignore the presence of the NOT_READY file in a project.")

        parser.add_option("--profile", action="store_true",
                          help="Print some profile informations about the execution.")

        parser.set_defaults(output=None,
                            mktemp=False,
                            append=False,
                            loglevel = WARNING,
                            disable_CASTOR=False,
                            dev_dirs=[],
                            ext_versions = {},
                            use = [],
                            tag_add = [],
                            set_CMTPATH = False,
                            runtime_projects = [],
                            overriding_projects = [],
                            auto_override = True,
                            use_grid = False,
                            quiet=False,
                            silent=False,
                            keep_CMTPROJECTPATH=False,
                            no_user_area=False,
                            )

        # FIXME: "from SP2.options ..." is enough with "python -m LbConfiguration.SetupProject"
        #        or with "python -c", but it does not work with our copy of runpy
        try:
            from LbConfiguration.SP2.options import addPlatform
            addPlatform(parser)
        except AssertionError:
            # LbConfiguration.SP2 works only with Python >= 2.6, but we need to
            # support 2.4 too (SLC5), so we ignore the assertion
            pass

        if 'CMTSITE' in self.environment and \
            self.environment["CMTSITE"] in self.default_externals:
            parser.set_defaults(site_externals = self.default_externals[self.environment["CMTSITE"]])
        else:
            parser.set_defaults(site_externals = self.default_externals['none'])

        return parser

    def _print_versions(self, versions):
        output = ''
        if not self.opts.quiet:
            for _p,v,_n,d in SortVersionTuples(versions):
                output += 'echo %s in %s\n' % (v, d)
        self._write_script(output)

    def _ask_version(self, versions):
        # ask for a version
        versions = SortVersions([ v[1] for v in versions ])
        # string representations, to handle correctly projects without version
        str_versions = [ str(v) for v in versions ]
        ans = None
        while not ans:
            sys.stderr.write("Please enter your choice (%s q[uit] [%s]): " %
                             (" ".join(str_versions), str_versions[-1]))
            sys.stderr.flush()
            ans = sys.stdin.readline().strip()
            if ans == '':
                ans = versions[-1]
            elif ans.lower() in [ 'q', 'quit' ]:
                return "quit"
            elif ans not in str_versions:
                self._always("Version '%s' not valid!", ans)
                ans = None
        if ans:
            self._always("Trying version '%s'"%ans)
            # handle project without version
            if ans == 'None' and None in versions:
                ans = None
        return ans

    def _touch_project_logfiles(self):
        touchline = ""
        self._debug("----- _touch_project_logfiles() -----")
        if self.shell in [ 'csh', 'sh' ]:
            # I have to touch a file to tell the release manager which version of the project I'm using
            if 'LHCB_USERLOGS' in self.environment and 'USER' in self.environment \
                and self.project_info.version \
                and lhcb_style_version.match(self.project_info.version): # I do not want to record non-standard or no versions
                dirname = self.environment['LHCB_USERLOGS']
                if os.path.isdir(dirname):
                    try:
                        subdir = os.path.join(dirname, self.project_info.name.upper())
                        if not os.path.exists(subdir):
                            os.mkdir(subdir)
                        touchline= 'touch %s/%s/%s_%s_%s >& /dev/null\n'%(dirname,
                                                  self.project_info.name.upper(),
                                                  self.project_info.name.upper(),
                                                  self.project_info.version,
                                                  self.environment["USER"])
                    except OSError:
                        # if we cannot create the directory, we do not touch the
                        # file
                        pass
        return touchline

    def _prepare_tmp_local_project(self):
        # prepare temporary local project directory
        self.tmp_root = TempDir(prefix = "SetupProject", keep_var = "SPKEEPTEMPDIR")

        tmp_dir = os.path.join(str(self.tmp_root),"v23") # add a fake version directory to please CMT
        os.mkdir(tmp_dir)

        self._debug("Using temporary directory '%s'"%tmp_dir)

        # prepare temporary requirements file
        req = open(os.path.join(tmp_dir,"requirements"),"w")

        # prepare use statementes for user-requested packages
        use_rexp = re.compile("^(?:([^/]*)/)?([^/ .]*)[ .]*([^ ]+)? *$")
        for u in self.opts.use:
            m = use_rexp.match(u)
            if m: # handle the case of "use" string in the format "[Hat/]Package [version]"
                hat, pack, ver = m.groups()
                if ver is None:
                    ver = 'v*'
                if hat is None:
                    try:
                        hat = LbConfiguration.Package.getPackage(pack).hat()
                    except:
                        # handles transparently the case the package doesn't exists
                        pass
                    hat = hat or '' # ensure that the hat is either 'not None' or the empty string
                req.write("use %s %s %s\n"%(pack,ver,hat))
            else: # for any other case, just use the provided string
                req.write("use %s\n"%u)

        if not self.external_only and self.project_info.sys:
            # if we were not asked only for external and the Sys package exists,
            # we use it
            req.write("use %s *\n" % self.project_info.sys)

        # add user specified tags
        for t in self.tag_add:
            req.write("apply_tag %s\n"%t)

        # prepare use statementes for externals (with default version)
        # version override have to happen later
        for e in self.args + self.ext_versions.keys() + self.site_externals:
            # @todo:I do not like too much this few lines
            if e != 'CASTOR' or not self.disable_CASTOR: # skip CASTOR
                req.write("use %s v* LCG_Interfaces\n"%e)

        # set config versions
        # (the override of the versions have to be done after the package:
        #  some packages may change the default version, but the command line
        #  must win)
        for e in self.ext_versions:
            req.write('macro %s_config_version "%s"\n'%(e,self.ext_versions[e]))

        # use external location relative to the LCG Interfaces for the local installation

        req.write('macro LCG_home "$(LCG_home)" LOCAL&Unix "$(LCG_SETTINGSROOT)/../../../.." LOCAL&WIN32 "$(LCG_SETTINGSROOT)\\..\\..\\..\\.."\n')

        req.flush()
        del req

        # prepare temporary project file
        os.mkdir(os.path.join(tmp_dir,"cmt"))
        prj = open(os.path.join(tmp_dir,"cmt","project.cmt"),"w")
        # add usage for the requested project(s)
        for p in self.overriding_projects + [self.project_info] + self.runtime_projects:
            prj.write("use %s\n" % p.realName.replace(os.sep," "))
        prj.flush()
        del prj

        if self._logger.level <= 2 :
            self._verbose("########## requirements ##########")
            self._verbose(open(os.path.join(tmp_dir,"requirements")).read())
            self._verbose("########## project.cmt ##########")
            self._verbose(open(os.path.join(tmp_dir,"cmt","project.cmt")).read())

        return tmp_dir

    def _gen_setup(self, root_dir):
        """
        Generate the setup script.
        Returns (script,error) where script is a string and error a list.
        If script is empty, you should find the reason in error.
        """
        self._debug("----- gen_setup() -----")

        if 'CMTPATH' not in self.environment \
            and 'CMTPROJECTPATH' not in self.environment:
            # it does not make sense to go on if the CMTPATH is not set
            raise SetupProjectError('neither CMTPATH nor CMTPROJECTPATH are set')

        if self.context_path:
            self.environment['CMTUSERCONTEXT'] = self.context_path

        # check CMT
        out = self.cmt("version", cwd = root_dir)
        if "command not found" in out:
            raise SetupProjectError('cmt is not in your PATH')
        else:
            self._debug("----- using CMT %s -----" % out.strip())

        # check if the project works
        out = self.cmt("check", "configuration", cwd = root_dir)
        if out and not self.force: # non empty output means error
            # ignore incompatibility notices
            inc_notice = re.compile(r'^# Required version.*incompatible with.*')
            if [l for l in out.splitlines() if not inc_notice.match(l)]:
                raise SetupProjectError(out)

        # Remove entries containing /gcc/ from the *PATH variables
        # https://its.cern.ch/jira/browse/LBCORE-899
        for v in ("PATH", "PYTHONPATH", "LD_LIBRARY_PATH", "ROOT_INCLUDE_PATH",
                  "HPATH", "MANPATH"):
            if v in self.environment:
                self.environment[v] = os.pathsep.join(x for x in self.environment[v].split(os.pathsep)
                                                      if '/gcc/' not in x)

        script = self.cmt("setup", "-" + self.shell, cwd = root_dir)

        #parse the output
        new_env = TemporaryEnvironment(self.environment)
        # this sets the new environment end return the line it cannot understand
        script = ShellParser[self.shell](script, new_env)

        # remove the temporary directory from the paths
        root_dir_local = root_dir
        if self.opts.shell == "bat":
            root_dir_local = root_dir_local.replace('/','\\')
        for v in ["PATH", "PYTHONPATH", "LD_LIBRARY_PATH", "HPATH", "MANPATH"]:
            if v in new_env:
                # Keep the entries that do not refer to the local temporary directory,
                # do exist and are not empty
                new_env[v] = os.pathsep.join([ d
                                               for d in new_env[v].split(os.pathsep)
                                               if root_dir_local not in d ])
                if "LB_NO_STRIP_PATH" not in os.environ:
                    self._debug("Cleaning variable %s", v)
                    self._debug("  before: %s", new_env[v])
                    new_env[v] = StripPath(new_env[v])
                    self._debug("   after: %s", new_env[v])
        # FIXME: I should look for all the variables pointing to the temporary directory

        # remove the variables that have the temporary directory in the name
        tmp_base_name = os.path.basename(str(self.tmp_root))
        for k in [ k
                   for k in new_env.keys()[:] # I need a copy of the keys
                   if tmp_base_name in k ]:
            del new_env[k]

        # remove CMTPATH if not explicitely requested
        if not self.set_CMTPATH and "CMTPATH" in new_env:
            del new_env["CMTPATH"]

        lines = []
        errors = []
        if self.context_path:
            if not self.opts.quiet :
                lines.append("echo Using CMTUSERCONTEXT = '%s'"%self.context_path)
            # unset CMTUSERCONTEXT in case of future calls
            del new_env['CMTUSERCONTEXT']

        for l in script.splitlines():
            if l.startswith("#CMT"):
                errors.append(l)
            else:
                lines.append(l)
        if lines: lines.append("")

        if (errors):
            self._debug("----- gen_setup(): errors != [] -----")
        # Do not forget changes in the environment
        new_env.commit()
        return ( "\n".join(lines), errors )

    def _prepend_prompt(self):
        new_prompt = ""
        if os.environ["SP_PROMPT"] == "without_version" or not self.project_info.version:
            if self.shell == "sh" :
                new_prompt  = 'if [[ -n "$PS1" ]]; then\n'
                new_prompt += '  PS1="\\[\\e[1;34m\\][%s]\\[\\e[m\\]$PS1"\n' % self.project_info.name
                new_prompt += 'fi\n'
            elif self.shell == "csh" :
                new_prompt  = 'if ( $?prompt ) then\n'
                new_prompt += '  set prompt="%%B%%{\\033[34m%%}[%s]%%{\\033[0m%%}%%b$prompt"\n' % self.project_info.name
                new_prompt += 'endif\n'
        else :
            if self.shell == "sh" :
                new_prompt  = 'if [[ -n "$PS1" ]]; then\n'
                new_prompt += '  PS1="\\[\\e[1;34m\\][%s %s]\\[\\e[m\\]$PS1"\n' % (self.project_info.name, self.project_info.version)
                new_prompt += 'fi\n'
            elif self.shell == "csh" :
                new_prompt  = 'if ( $?prompt ) then\n'
                new_prompt += '  set prompt="%%B%%{\\033[34m%%}[%s %s]%%{\\033[0m%%}%%b$prompt"\n' % (self.project_info.name, self.project_info.version)
                new_prompt += 'endif\n'

        return new_prompt

    def parse_args(self,args = sys.argv[1:]):
        self.opts,self.args = self.parser.parse_args(args=args)

    def prepare(self, args = None):
        """
        Collect all the informations needed to set up the environment, like
        project name and version, paths, etc.
        """
        # If the option --silent is specified, --quiet is implied
        if self.opts.silent:
            self.opts.quiet = True

        # set level of log messages
        if not self.opts.quiet :
            self._logger.level = self.loglevel
        elif self.opts.silent:
            self._logger.level = ALWAYS + 10 # do not even print ALWAYS messages
        else: # quiet but not silent
            self._logger.level = ERROR
        log.level = self.loglevel

        self._debug("----- main() -----")

        self._verbose("SetupProject %s"%self.parser.get_version())

        #------------- get project name
        if len(self.args) < 1:
            self._error("You have to specify a project")
            return 1

        required_root_version = None
        if self.args[0].lower() == "root":
            if self.list_versions:
                self._error('cannot list the versions of ROOT')
                return 1
            # ROOT is not a CMT project, instead of
            #   SetupProject ROOT 5.34.10
            # we should do
            #   SetupProject LCGCMT 66 ROOT
            self.project_name = "LCGCMT"
            # let's see if the user actually passed us a version for ROOT
            if len(self.args) > 1 and re.match(r"\d+\.\d+\.\d+", self.args[1]):
                # yes, record it for later (when we have the search path ready)
                required_root_version = self.args[1]
                # fix the arguments
                self.args[0:2] = ['ROOT']
            else:
                self._debug('Using the latest version of LCGCMT to provide ROOT')
                # if the version is not given, we do not need to do anything
                # special on the arguments, just ensure that the case is correct
                self.args[0] = "ROOT"
        else:
            self.project_name = FixProjectCase(self.args.pop(0))

        #------------- sanity check: the shell type has to be specified
        if not self.shell:
            self._error("Internal error: shell type not specified")
            return 1

        #------------- set user area
        if self.opts.no_user_area:
            self.user_area = None
        elif self.opts.user_area:
            self.user_area = self.opts.user_area
        else:
            self.user_area = self.environment.get('User_release_area', None)

        #------------- prepare search_path
        self.search_path = []
        # user area
        if self.user_area:
            self.search_path.append(self.user_area)
        # dev dirs
        if self.dev_dirs:
            self.search_path += self.dev_dirs
        # default locations
        projpathvars = ["LHCBPROJECTPATH"]
        if self.keep_CMTPROJECTPATH:
            projpathvars.insert(0, "CMTPROJECTPATH")
        for v in projpathvars:
            if v in self.environment:
                self.search_path += self.environment[v].split(os.pathsep)

        # remove duplicates
        self.search_path = uniq(self.search_path)
        # remove entries that do not exist (bug #75480)
        self.search_path = filter(os.path.exists, self.search_path)

        #------------- discover all project versions
        # debug printout: print project, search path and, optionally, user area
        self._debug("Look for all versions of '%s' in %s" % (self.project_name, self.search_path)
                    + ((self.user_area and (" with user area in '%s'" % self.user_area)) or ""))
        versions = FindProjectVersions(self.project_name, self.search_path,
                                       self.user_area, self.opts.ignore_not_ready)

        if not versions:
            self._error("Cannot find project '%s'" % self.project_name)
            return 1

        # Now I can print the version if I was asked to do it
        if self.list_versions:
            self._print_versions(versions)
            return 0

        if required_root_version:
            # now we can look for the version of LCGCMT that matches the required
            # version of ROOT
            self._debug('Looking for the LCGCMT version providing ROOT %s' %
                        required_root_version)
            lcg_version = FindLCGForROOT(required_root_version,
                                         SortVersionTuples(versions, reverse=True))
            if lcg_version:
                # we need the version in the args because it's processed later
                self.args.insert(0, lcg_version)
            else:
                self._error("Cannot find ROOT %s" % required_root_version)
                return 1

        #------------- project version
        if self.args:
            # check if the next argument can be interpreted as a version for the requested project
            found = False
            pattern = self.args[0]
            if pattern.upper() == 'HEAD':
                pattern = self.args[0] = 'HEAD'
            for v in versions:
                if VersionMatch(v[1], pattern):
                    found = True
                    break
            if found:
                self.project_version = self.args.pop(0)
            elif re.match('v[0-9]*r[0-9p]*|HEAD', self.args[0]):
                self._error("Cannot find version '%s' of %s. Try with --list-versions." % (self.args[0], self.project_name))
                return 1

        #------------- Prompt for a version to use
        # Ask if --ask or with --build-env and no version specified
        if self.ask or (self.build_env and not self.project_version):
            v = self._ask_version(versions)
            if v == "quit":
                self._always("Quit")
                return 1
            self.project_version = v

        #------------- Initialize the ProjectInfo objects
        # Main project
        self.project_info = makeProjectInfo(version = self.project_version,
                                            versions = versions,
                                            env = self.environment,
                                            ignore_not_ready = self.opts.ignore_not_ready)
        if not self.project_info:
            # we should never get here
            self._error("PANIC: Cannot find version '%s' of %s after initial check" % (self.project_version, self.project_name))
            return 1

        # runtime projects
        self.runtime_projects = []
        for p,v in self.opts.runtime_projects:
            vv = FindProjectVersions(p, self.search_path,
                                     self.user_area, self.opts.ignore_not_ready)
            if not vv:
                self._error("Cannot find project '%s'" % p)
                return 1
            pi = makeProjectInfo(version = v,
                                 versions = vv,
                                 env = self.environment,
                                 ignore_not_ready = self.opts.ignore_not_ready)
            if not pi:
                self._error("Cannot find version '%s' of %s. Try with --list-versions." % (v, p))
                return 1
            self.runtime_projects.append(pi)

        # overriding projects
        self.overriding_projects = []
        for p,v in self.opts.overriding_projects:
            vv = FindProjectVersions(p, self.search_path,
                                     self.user_area, self.opts.ignore_not_ready)
            if not vv:
                self._error("Cannot find project '%s'" % p)
                return 1
            pi = makeProjectInfo(version = v,
                                 versions = vv,
                                 env = self.environment,
                                 ignore_not_ready = self.opts.ignore_not_ready)
            if not pi:
                self._error("Cannot find version '%s' of %s. Try with --list-versions." % (v, p))
                return 1
            self.overriding_projects.append(pi)

        # use LHCbGrid
        if self.opts.use_grid:
            auto_override_projects.append( ("LHCbGrid", ["LHCbGridSys"]) )

        # auto-override projects
        if self.auto_override:
            for p, pkgs  in auto_override_projects:
                vv = FindProjectVersions(p, self.search_path,
                                         self.user_area, self.opts.ignore_not_ready)
                if vv:
                    self.overriding_projects.insert(0, makeProjectInfo(versions = vv,
                                                                       env = self.environment,
                                                                       ignore_not_ready = self.opts.ignore_not_ready))
                    self.opts.use += pkgs

        platform = self.opts.platform
        for p in self.overriding_projects + [self.project_info] + self.runtime_projects :
            if not p.supportsPlatform(platform, self.user_area):
                self._error("Project %s %s is not available for platform %s" % (p.name, p.version, platform))
                return 1
            self._verbose("Project %s %s uses %s policy"%(p.name,
                                                          p.version,
                                                          p.policy))

        if self.opts.platform:
            self.environment['CMTCONFIG'] = self.opts.platform

        return 0

    def main(self, args = None):
        """
        Main entry point to SetupProject script.

        Calls parse_args, then the 'run' method, profiling it if requested.
        """
        # initialize the logger (print on standard error)
        log_handler = logging.StreamHandler(sys.stderr)
        log_handler.setFormatter(logging.Formatter("%(message)s"))
        logging.getLogger().addHandler(log_handler)

        if args is None:
            args = sys.argv[1:]
        # Process commmand line options
        self.parse_args(args=args)

        # Prepare the profiler instance
        profiler = None
        if self.opts.profile:
            try:
                import cProfile
                profiler = cProfile.Profile()
            except ImportError:
                try:
                    import profile
                    profiler = profile.Profile()
                except ImportError:
                    self._warning("Cannot import cProfile or profile: ignoring --profile")
        # if we managed to get the profiler instance, collect profiling stats,
        # and print them on stderr
        if profiler:
            rc = profiler.runcall(self.run)
            # adapted from Python standard profiler.py
            import pstats
            if sys.version_info < (2, 5):
                # hack to be able to print the statistics on stderr on older Python
                stats = pstats.Stats(profiler)
                sys.stdout = sys.stderr
            else:
                stats = pstats.Stats(profiler, stream=sys.stderr)
            stats.strip_dirs().sort_stats('cumulative').print_stats(10)
        else:
            rc = self.run()

        return rc

    def run(self, args = None):
        # Initialization from arguments
        rc = self.prepare(args)
        if rc or self.list_versions: # No need to go on if --list-versions or error
            return rc

        # Prepare the build-time environment
        self.set_build_env()
        # We usually unset CMTPATH, but we need to remember it for old style
        # projects
        CMTPATH = self.environment.get("CMTPATH","")

        script = "" # things we need to append to the setup script (like aliases)
        messages = [] # lines to print (feedback)
        if not self.build_env: # this part is needed only if we do not ask for build only env
            if not self.opts.quiet:
                self._always("Configuring %s" % self.project_info)
            tmp_dir = self._prepare_tmp_local_project()
            try:
                (script,err) = self._gen_setup(tmp_dir)
                if err and self.force:
                    self._always("WARNING - ignoring warnings and errors from CMT:")
                    self._always("\n".join(err))
            except SetupProjectError, x:
                self._error(str(x))
                if 'package CASTOR' in str(x):
                    self._error("Tip: try adding --disable-CASTOR to the options")
                self._error("Could not produce the environment, check the arguments")
                return 1
            # Feedback
            if 'CMTPROJECTPATH' in self.environment:
                messages.append("Using CMTPROJECTPATH = '%(CMTPROJECTPATH)s'" % self.environment)
                if self.keep_CMTPROJECTPATH:
                    messages.append("(as defined by the user)")
            else:
                messages.append("Using CMTPATH = '%s'" % CMTPATH)
            tmps = self.project_info.name
            if self.project_info.version: tmps += " %s" % self.project_info.version
            messages.append('Environment for %s ready.' % tmps)

            # collect project infos descriptions
            lines = []
            for pi in self.overriding_projects:
                lines.append(str(pi))
            lines.append(str(self.project_info))
            for pi in self.runtime_projects:
                lines.append(str(pi))
            # add decorators
            if len(lines) > 1:
                lines = [ '(%s,' % lines[0] ] + [ ' %s,' % l for l in lines[1:-1] ] + [ ' %s)' % lines[-1] ]
            else:
                lines[0] = '(%s)' % lines[0]
            messages += lines

            # FIXME: Hack to hide the fact that old projects were not setting the PATH for the executable
            if self.project_info.policy == 'old':
                varname = self.project_info.name.upper() + "ROOT"
                if varname in self.environment:
                    exedir = os.path.join(self.environment[varname],
                                          self.opts.platform)
                    if os.path.isdir(exedir):
                        # it make sense to add it only if it exists
                        messages.append('Appending %s to the path.' % exedir)
                        self.environment["PATH"] = os.pathsep.join([self.environment["PATH"], exedir])

        else: # Prepare build-time environment
            tmps = self.project_info.name
            if self.project_info.version: tmps += " %s" % self.project_info.version
            messages.append('Build-time environment for %s ready.' % tmps)
            # create the user project if User_release_area is defined, it is a
            # project with InstallArea and the directory doesn't exist yet.
            if self.user_area:
                if self.project_info.policy != 'old':
                    user_proj_name = "%s_%s" % (self.project_info.name, self.project_info.version)
                    user_proj_dir = os.path.join(self.user_area, user_proj_name)
                    if not os.path.exists(user_proj_dir):
                        self.cmt("create_project",
                                 [user_proj_name,
                                  "-use=%s" % self.project_info.realName.replace(os.path.sep,":")],
                                 cwd = self.user_area)
                        if os.path.isdir(user_proj_dir):
                            messages.append('Created user project in %s' % user_proj_dir)
                            # Add the structuring style to the local project
                            proj_file = open(os.path.join(user_proj_dir, "cmt", "project.cmt"), "a")
                            proj_file.writelines(["build_strategy with_installarea\n",
                                                  "structure_strategy without_version_directory\n"])
                            proj_file.close()
                            del proj_file
                        else:
                            messages.append('Cannot create user project in %s' % user_proj_dir)
                    # do the rest only if the user project directory is present
                    if os.path.isdir(user_proj_dir):
                        # Create a project Makefile
                        createProjectMakefile(os.path.join(user_proj_dir, "Makefile"), overwrite = False)
                        # eclipse configuration
                        createEclipseConfiguration(user_proj_dir, self.environment["CMTPROJECTPATH"])
                        # Let's enter the user project directory
                        script += "cd %s\n" % user_proj_dir
                        messages.append("Current directory is '%s'." % user_proj_dir)
            if 'CMTPROJECTPATH' in self.environment:
                messages.append("Using CMTPROJECTPATH = '%(CMTPROJECTPATH)s'" % self.environment)
            else:
                messages.append("Using CMTPATH = '%s'" % CMTPATH)

        # normalize relative path introduced for the location the externals relatively
        # to the LCGCMT project
        path2normalize = ["PYTHONPATH", "PATH", "LD_LIBRARY_PATH", "ROOTSYS"]
        for p in path2normalize :
            if p in self.environment.keys() :
                pthlist = self.environment[p].split(os.pathsep)
                newlist = []
                for l in pthlist :
                    newpath = os.path.normpath(l)
                    if os.path.exists(newpath) and l.find("..") != -1 :
                        newlist.append(newpath)
                    else :
                        newlist.append(l)
                self.environment[p] = os.pathsep.join(newlist)

        output_script = self.environment.gen_script(self.shell)
        output_script += script
        if not self.opts.quiet :
            for m in messages:
                output_script += 'echo "%s"\n' % m
        if self.opts.touch_logfile:
            #I have to touch a file to tell the release manager which version of the project I'm using
            output_script += self._touch_project_logfiles()

        if os.environ.has_key("SP_PROMPT") and not self.build_env:
            output_script += self._prepend_prompt()

        self._verbose("########## done ##########")

        # as very last step, generate the output script to be sourced by the wrapper
        self._write_script(output_script)

        ### completed successfully
        return 0

if __name__ == '__main__':
    sys.exit(SetupProject().main(sys.argv[1:]))

