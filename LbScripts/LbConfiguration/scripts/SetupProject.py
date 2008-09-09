#!/usr/bin/env python

import os, sys, tempfile, re, sys
from stat import S_ISDIR
import getopt

_cvs_id = "$Id: SetupProject.py,v 1.24 2008-09-09 11:37:06 hmdegaud Exp $"

########################################################################
# Useful constants
########################################################################
lhcb_style_version = re.compile(r'v([0-9]+)r([0-9]+)(?:p([0-9]+))?')
lcg_style_version = re.compile(r'([0-9]+)([a-z]?)')
# TODO: Copied from LbConfiguration.Project
project_names = ["Gaudi", "LHCb", "Lbcom", "Rec", "Boole", "Brunel" , 
                 "Gauss", "Phys", "Analysis", "Hlt", "Alignment", "Moore",
                 "Online", "Euler", "Geant4", "DaVinci", "Bender", "Orwell",
                 "Panoramix", "LbScripts", "Curie", "LCGCMT"]

# List of pairs (project,[packages]) to automatically select for override
# The project are prepended to the list of overriding packages and 
# the packages are appended to the list of used packages
auto_override_projects = [("ExtraPackages", []),
                          #("LHCbGrid", ["LHCbGridSys"]), # enabled with --use-grid
                          ]

########################################################################
# Utility classes
########################################################################
class Logger:
    DEBUG   = 1
    VERBOSE = 2
    INFO    = 3
    WARNING = 4
    ERROR   = 5
    ALWAYS  = 6
    def __init__(self, out = None):
        if out is None:
            out = sys.stderr
        self.out = out
        self.level = 3
    def write(self, level, s):
        if level >= self.level:
            self.out.write(s)
            self.out.flush()
    def writeln(self, level, s):
        if level >= self.level:
            self.out.write(s)
            self.out.write("\n")
            self.out.flush()
    def debug(self, message):
        self.writeln(self.DEBUG, message)
    def verbose(self, message):
        self.writeln(self.VERBOSE, message)
    def info(self, message):
        self.writeln(self.INFO, message)
    def warning(self, message):
        self.writeln(self.WARNING, message)
    def error(self, message):
        self.writeln(self.ERROR, message)
    def always(self, message):
        self.writeln(self.ALWAYS, message)

log = Logger()

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
        return self.env[key]
    
    def __delitem__(self,key):
        """
        Unset an environment variable.
        Needed to provide the same interface as os.environ.
        """
        if key not in self.env :
            raise KeyError(key)
        self.old_values[key] = self.env[key]
        del self.env[key]

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
        shells = [ 'csh', 'sh', 'bat' ]
        if shell_type not in shells:
            raise RuntimeError("Shell type '%s' unknown. Available: %s" % (shell_type, shells))
        out = ""
        for key in self.old_values:
            if key not in self.env:
                # unset variable
                if shell_type == 'csh':
                    out += 'unsetenv %s\n' % key
                elif shell_type == 'sh':
                    out += 'unset %s\n' % key
                elif shell_type == 'bat':
                    out += 'set %s=\n' % key
            else:
                # set variable
                if shell_type == 'csh':
                    out += 'setenv %s "%s"\n' % (key, self.env[key])
                elif shell_type == 'sh':
                    out += 'export %s="%s"\n' % (key, self.env[key])
                elif shell_type == 'bat':
                    out += 'set %s=%s\n' % (key, self.env[key])
        return out

class TempDir:
    def __init__(self, keep = False):
        self.name = mkdtemp()
        self._keep = keep
        
    def __str__(self):
        return self.name
    
    def __del__(self):
        if self.name:
            if "KEEPTEMPDIR" in os.environ:
                log.always("KEEPTEMPDIR set: I do not remove the temporary directory '%s'" % self.name)
                return
            removeall(self.name)
    
    def __getattr__(self,attr):
        return getattr(self.name,attr)

def isProject(path):
    return os.path.isfile(os.path.join(path,'cmt','project.cmt'))

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

def FindProjectVersions(project, search_path, user_area = None):
    """Given a project name, discovers all the matching project/versions using
    the provided user_area and the the search_path (a list of directories).
    Returns a list of tuples with (name, version, realname, basepath). If there
    is no version the middle entry is None.
    """ 
    env = os.environ
    versions = [] # container of the results
    # Look into user_area for projects (without version directory)
    if user_area and os.path.isdir(user_area):
        # look for projects with names starting with <project>
        candidates = [d for d in os.listdir(user_area)
                        if isProject(os.path.join(user_area,d)) and 
                           d.startswith(project)]
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
        p = os.path.join(d,PROJECT) 
        if os.path.isdir(p):
            if isProject(p):
                # project without version
                versions.append((project, None, PROJECT, d))
            else:
                # project with versions
                versions += [ (project, _extract_version(PROJECT,v),
                               os.path.join(PROJECT,v), d)
                               for v in os.listdir(p)
                               if isProject(os.path.join(p,v)) ]
    return versions

def SortVersions(versions):
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
    sortable_list.sort()
    return [ v[1] for v in sortable_list ]


def LatestVersion(versions):
    """Extract the latest version from a list of version tuples
    (output of FindProjectVersions)."""
    return SortVersions([ v[1] for v in versions ])[-1]

def _GetVersionTuple(v, versions):
    """Extract the version tuple corresponding to version v.
    """
    if not versions:
        return None
    for vers_tuple in versions:
        if vers_tuple[1] == v:
            return vers_tuple
    if v is None: # take the latest
        return _GetVersionTuple(LatestVersion(versions), versions)
    return None

class ProjectInfo:
    """
    Class to get project related infos.
    """
    # @todo: use LHCB_config.py instead of scanning directories
    searchpath = []
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
        
        self.sys = self._projectsys()
        if self.version and self.sys:
            self.syscmtfullpath = os.path.join(self.project_dir,self.sys,self.version,'cmt')
        else:
            self.syscmtfullpath = None
        
    def _projectsys(self):
        """
        Return the name of the package referencing all the packages belonging to a project.
        """
        if self.name == 'Gaudi':
            if self.policy == 'new':
                return self.name + "Release"
            else:
                return self.name
        elif self.name == 'LCGCMT':
            return None
        else:
            return self.name + 'Sys'
    def __str__(self):
        if self.version:
            return "%s %s from %s" % (self.name, self.version, self.project_dir)
        else:
            return "%s from %s" % (self.name, self.project_dir)

def _defaultSearchPath():
    search_path = []
    for v in ["CMTPROJECTPATH", "LHCBPROJECTPATH"]:
        if v in os.environ:
            search_path += os.environ[v].split(os.pathsep)
    return search_path

def makeProjectInfo(project = None, version = None, versions = None, search_path = None, user_area = None):
    # actual body
    if versions is None:
        if not project:
            raise TypeError("makeProjectInfo() requires either 'project' or 'versions'")
        if not search_path: # default search path
            search_path = _defaultSearchPath()
        versions = FindProjectVersions(project, search_path, user_area)
    vers_tuple = _GetVersionTuple(version,versions)
    if not vers_tuple:
        return None
    return apply(ProjectInfo,vers_tuple)

class CMT:
    def __init__(self):
        pass
    def _run_cmt(self,command,args):
        if type(args) is str:
            args = [args]
        cmd = "cmt %s"%command
        for arg in args:
            cmd += ' "%s"'%arg
        #print cmd
        return os.popen4(cmd)[1].read()
    def __getattr__(self,attr):
        return lambda args=[]: self._run_cmt(attr, args)
    def show_macro(self,k):
        r = self.show(["macro",k])
        if r.find("CMT> Error: symbol not found") >= 0:
            return None
        else:
            return self.show(["macro_value",k]).strip()

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

if not 'mkdtemp' in dir(tempfile):
    # mkdtemp has been introduced in python 2.3, I simulate it
    import warnings
    warnings.filterwarnings(action = 'ignore', message = '.*tmpnam.*', category = RuntimeWarning)
    def mkdtemp():
        name = os.tmpnam()
        os.mkdir(name,0700)
        return name
    def mkstemp():
        name = os.tmpnam()
        return (os.open(name,os.O_CREAT | os.O_RDWR | os.O_EXCL, 0600), name)
else:
    # use the real mkdtemp
    from tempfile import mkdtemp, mkstemp

def removeall(path):
    """
    Recursively remove directories and files.
    """
    log.writeln(log.DEBUG,"----- removeall(%s) -----"%path)
    if S_ISDIR(os.stat(path).st_mode):
        lst = os.listdir(path)
        for p in lst:
            removeall(os.path.join(path,p))
        os.rmdir(path)
    else:
        os.remove(path)

def _get_cmt_user_context():
    log.writeln(log.DEBUG,"----- get_cmt_user_context() -----")
    if 'CMTPATH' not in os.environ:
        # This function needs CMTPATH, which means that it does not work with CMTPROJECTPATH
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): no CMTPATH -----")
        return None # For the moment we ignore the CMTUSERCONTEXT
    
    # check if we can use CMTUSERCONTEXT
    context_path = None
    context_validity = None
    current_lcgcmt = None
    
    # Find the context validity
    if os.environ['LHCb_release_area']:
        context_path = os.path.join(os.environ['LHCb_release_area'],'context')
        context_file = os.path.join(context_path,'requirements')
        
        if os.access(context_file,os.F_OK | os.R_OK):
            # find the magic comment
            for l in open(context_file).xreadlines():
                m = re.match("^#.*created.*from.*LCGCMT_(([0-9]+)([a-z]?)|HEAD)", l)
                if m :
                    context_validity = m.groups()
                    break
                
    if not context_validity:
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): context not found -----")
        return None
    else:
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): context if from LCGCMT %s -----"%(context_validity[0]))
        
    # we have to compare the version with the current one
    for l in os.environ['CMTPATH'].split(os.pathsep):
        m = re.match("^.*LCGCMT_(([0-9]+)([a-z]?)|HEAD)", l)
        if m:
            current_lcgcmt = m.groups()
            break
    if current_lcgcmt:
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): Default LCGCMT is version %s -----"%(current_lcgcmt[0]))
    
    if not current_lcgcmt:
        # for no lcgcmt in the path, assume we need the context
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): LCGCMT not found, use %s -----"%context_path)
        return context_path

    # at this point we have both context and current: comparison time
    if context_validity[0] == current_lcgcmt[0] or current_lcgcmt[0] == 'HEAD':
        # the current is good
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): current LCGCMT is good -----")
        return None

    if context_validity[0] == 'HEAD':
        # the current cannot be better
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): use %s -----"%context_path)
        return context_path

    if int(context_validity[1]) > int(current_lcgcmt[1]):
        # context is better
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): use %s -----"%context_path)
        return context_path
    elif (context_validity[1] == current_lcgcmt[1]) and (context_validity[2] > current_lcgcmt[2]):
        # like in 46b > 46a.. context is still better
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): use %s -----"%context_path)
        return context_path
    else:
        # it seems that the current one is better
        log.writeln(log.DEBUG,"----- get_cmt_user_context(): do not use the context -----")
        return None

def shParser(script, env):
    leftover = []
    for l in script.splitlines():
        if l.startswith("unset"):
            var = l.split()[-1]
            if var in env:
                del env[var]
        elif "export" in l:
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
class SetupProject:
    def __init__(self):
        self._logger = Logger()
        self.cmt = CMT()
        
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
                                  'CERN':['CASTOR'],
                                 }
        self.parser = self._prepare_parser()
        
    def __getattr__(self,attr):
        """
        Shortcut to retrieve the options
        """
        if self.opts:
            return getattr(self.opts,attr)
        raise AttributeError("'%s' object has no attribute '%s'"%
                             (self.__class__.__name__,attr))
    
    def _log(self,lvl,msg):
        self._logger.writeln(lvl,msg)
    def _always(self,msg):
        self._log(self._logger.ALWAYS, msg)
    def _error(self,msg):
        self._log(self._logger.ERROR, msg)
    def _warning(self,msg):
        self._log(self._logger.WARNING, msg)
    def _info(self,msg):
        self._log(self._logger.INFO, msg)
    def _verbose(self,msg):
        self._log(self._logger.VERBOSE, msg)
    def _debug(self,msg):
        self._log(self._logger.DEBUG, msg)
    
    def check_environment(self):
        """
        Check that the current environment is usable.
        """
        self._debug("----- check_environment() -----")
        output = self.cmt.version()
        if output.find('command not found') > 0 or output.find('not recognized') > 0 :
            self._error("I cannot find cmt (it is not in your PATH)")
            return False
        if not 'CMTCONFIG' in os.environ:
            self._error("You do not have CMTCONFIG set")
            return False
        return True
    
    def set_build_env(self, env):
        self._debug("----- set_build_env() -----")
        if not self.project_info:
            # do nothing
            # FIXME: is this an error?
            return
        if self.project_info.policy =='old':# old style project
            self._debug("----- old style project -----")
            olddir = os.getcwd()
            os.chdir(self.project_info._projectenv_cmt_dir)
            if 'CMTPROJECTPATH' in env:
                self._debug("----- unsetenv CMTPROJECTPATH -----")
                del env['CMTPROJECTPATH']
            env['CMTPATH'] = os.popen("cmt show set_value CMTPATH").readline().strip()
            os.chdir(olddir)
            if 'User_release_area' in env:
                if env['CMTPATH'].find(env['User_release_area']) < 0:
                    env['CMTPATH'] =  os.pathsep.join([ env['User_release_area'],
                                                        env['CMTPATH'] ])
            self._debug("----- CMTPATH set to '%s' -----"%env['CMTPATH'])
        else:
            # split the CMTPROJECTPATH in a convenient way
            cmtProjPath = env.get("CMTPROJECTPATH","")
            if cmtProjPath:
                cmtProjPath = cmtProjPath.split(os.pathsep)
            else: # to prevent a dir of size 0 in the path
                cmtProjPath = []
            # Prepend the user release area, if not there
            if self.user_area and self.user_area not in cmtProjPath:
                cmtProjPath.insert(0, self.user_area)
            # explicit dev dirs must always be taken into account
            for d in self.dev_dirs:
                if d not in cmtProjPath:
                    cmtProjPath.insert(0,d)
            # the project directory should be appended, if not there
            if self.project_info.path not in cmtProjPath:
                cmtProjPath.append(self.project_info.path)
            # append search_path directories too, to be sure that everything is there
            for d in self.search_path:
                if d not in cmtProjPath:
                    cmtProjPath.append(d)
                    
            env['CMTPROJECTPATH'] = os.pathsep.join(cmtProjPath)
            self._debug("----- CMTPROJECTPATH set to '%s' -----"%env['CMTPROJECTPATH'])
            
            # unset CMTPATH if present
            if 'CMTPATH' in env:
                self._debug("----- unsetenv CMTPATH -----")
                del env['CMTPATH']
        
    def _write_script(self,data):
        close_output = False
        if self.output:
            self.output_file = open(self.output,"w")
            self.output = None # reset the option value to avoid to reuse it
            close_output = True
        elif self.mktemp:
            fd, outname = mkstemp()
            self.output_file = os.fdopen(fd,"w")
            print outname
            self.mktemp = None # reset the option value to avoid to reuse it
            close_output = True
        # write the data
        self.output_file.write(data)
        self.output_file.write("\n") # @todo: this may be avoided
        if close_output: self.output_file.close()
    
    def _prepare_parser(self):
        if sys.version_info < (2,3,0,0,0):
            # optparse is not available before Python 2.3
            # since we should be in the LHCb environment, we should be able
            # to find Python 2.4.2
            if os.environ["LCG_release_area"].find("external") >= 0:
                # local installation
                lcg_externals = os.environ["LCG_release_area"]
            else:
                # AFS installation
                lcg_externals = os.path.join(os.environ["LCG_release_area"],
                                             "..","..","external")
                lcg_externals = os.path.normpath(lcg_externals)
            lcg_sys = os.environ["CMTCONFIG"]
            if lcg_sys.endswith("_dbg"):
                lcg_sys = lcg_sys[:-4]
            lcg_python = os.path.join(lcg_externals,"Python","2.4.2",lcg_sys,
                                      "lib","python2.4")
            if os.path.isdir(lcg_python):
                sys.path.append(lcg_python)
            else:
                self._error("ERROR - SetupProject needs Python >= 2.3")
                sys.exit(1)
        from optparse import OptionParser, \
                             IndentedHelpFormatter as BasicHelpFormatter,\
                             Option, \
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

#        class MyOption(Option):
#            def take_action(self, action, dest, opt, value, values, parser):
#                if action == "help":
#                    parser.print_help(sys.stderr)
#                    #parser.exit() # this is python 2.4
#                    sys.exit(0)
#                elif action == "version":
#                    parser.print_version(sys.stderr)
#                    #parser.exit() # this is python 2.4
#                    sys.exit(0)
#                return Option.take_action(self, action, dest, opt, value, values, parser)
#        
#        parser = OptionParser(usage = "%prog [options] <project_name> [version|--ask] [options] [externals]",
#                              formatter = MyHelpFormatter(),
#                              option_class = MyOption,
#                              add_help_option=False)
#        parser.version = _cvs_id
#                              
#        parser.add_option(MyOption("-h", "--help",
#                                   action="help",
#                                   help="show this help message and exit"))
#        parser.add_option(MyOption("--version",
#                                   action="version",
#                                   help="show program's version number and exit"))
#
        class MyOptionParser(OptionParser):
            def print_help(self,file = None):
                if file is None:
                    file = sys.stderr
                return OptionParser.print_help(self,file)
            def print_version(self,file = None):
                if file is None:
                    file = sys.stderr
                return OptionParser.print_version(self,file)
        
        parser = MyOptionParser(usage = "%prog [options] <project_name> [version|--ask] [options] [externals]",
                                version = _cvs_id,
                                formatter = MyHelpFormatter())
        
        def check_output_options_cb(option, opt_str, value, parser):
            if opt_str == "--mktemp":
                if parser.values.output:
                    raise OptionValueError("--mktemp cannot be used at the same time as --output")
                parser.values.mktemp = True
            elif opt_str == "--output":
                if parser.values.mktemp:
                    raise OptionValueError("--mktemp cannot be used at the same time as --output")
                parser.values.output = value
        
        # internal options
        parser.add_option("--shell", action="store", type="choice", metavar="SHELL",
                          choices = ['csh','sh','bat'],
                          help="(internal) select the type of shell to use")
        parser.add_option("--output", action="callback", metavar="FILE",
                          type = "string", callback = check_output_options_cb,
                          help="(internal) output the command to set up the environment ot the given file instead of stdout")
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
                          dest="loglevel", const=2, 
                          help="be a bit more verbose")
        parser.add_option("--debug", action="store_const",
                          dest="loglevel", const=1, 
                          help="output useful for debugging")
        parser.add_option("--ignore-missing", action="store_true",
                          help="do not fail if some externals are missing, just complain")
        parser.add_option("--ignore-context", action="store_true",
                          help="do not use CMTUSERCONTEXT even if it should be used")
        parser.add_option("--list-versions", action="store_true",
                          help="print available versions of the specified project and exit (all other options are ignored)")
        parser.add_option("--build-env", action="store_true",
                          help="sets only the build time environment for the project")
        parser.add_option("--external-only", action="store_true",
                          help="sets only the environment for the externals (the project is used only to select the version of LCG)")
        
        def dev_dir_cb(option, opt_str, value, parser):
            parser.values.dev = True
            if value is None:
                if "LHCBDEV" in os.environ:
                    value = os.environ["LHCBDEV"]
                else:
                    raise OptionValueError("--dev used, but LHCBDEV is not defined")
            parser.values.dev_dirs.append(value)
        parser.add_option("--dev", action="callback",
                          callback = dev_dir_cb,
                          help="prepend $LHCBDEV to the search path or DEVDIR if the option --dev-dir is specified")
        parser.add_option("--dev-dir", action="callback", metavar="DEVDIR",
                          type = "string", callback = dev_dir_cb,                
                          help="define a custom DEVDIR to be used instead of $LHCBDEV, implies --dev")
    
        def external_version_option(option, opt_str, value, parser):
            if len(parser.largs) < 2:
                raise OptionValueError("%s must be preceded by the name of the external"%opt_str)
            parser.values.ext_versions[parser.largs.pop()] = value
            
        parser.add_option("-v", action="callback", metavar="VERSION",
                          type="string", callback=external_version_option,
                          help="must be used after the name of an external to specify a non default version for it")
    
        parser.add_option("--set-CMTPATH", action="store_true",
                          help="Set CMTPATH to the value used internally by CMT (DANGEROUS)")

        
        def runtime_project_option(option, opt_str, value, parser):
            if len(parser.rargs) < 1:
                raise OptionValueError("%s must be followed by the project name and optionally by the version"%opt_str)
            p_name = parser.rargs.pop(0)
            if len(parser.rargs) and re.match("^v[0-9]+r[0-9]+(p[0-9]+)?$",parser.rargs[0]):
                v = parser.rargs.pop(0)
            else:
                v = None
            parser.values.runtime_projects.append( (p_name,v) )
            
        parser.add_option("--runtime-project", action="callback",
                          metavar = "PROJECT [VERSION]", type="string",
                          callback=runtime_project_option,
                          nargs = 0,
                          help="Add a project to the runtime environment")
        
        def overriding_project_option(option, opt_str, value, parser):
            if len(parser.rargs) < 1:
                raise OptionValueError("%s must be followed by the project name and optionally by the version"%opt_str)
            p_name = parser.rargs.pop(0)
            if len(parser.rargs) and re.match("^v[0-9]+r[0-9]+(p[0-9]+)?$",parser.rargs[0]):
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
        
        parser.add_option("--silent", action="store_true",
                          help = "Removes message printout during setup")
        
        
        parser.set_defaults(output=None,
                            mktemp=False,
                            loglevel = 3,
                            disable_CASTOR=False,
                            dev=False,
                            dev_dirs=[],
                            ext_versions = {},
                            use = [],
                            tag_add = [],
                            set_CMTPATH = False,
                            runtime_projects = [],
                            overriding_projects = [],
                            auto_override = True,
                            use_grid = False,
                            silent=False
                            )
        
        if 'CMTSITE' in os.environ and \
            os.environ["CMTSITE"] in self.default_externals:
            parser.set_defaults(site_externals = self.default_externals[os.environ["CMTSITE"]])
        else:
            parser.set_defaults(site_externals = self.default_externals['none'])
        
        return parser
    
    def _print_versions(self, versions):
        output = ''
        vers_locs = {}
        for p,v,n,d in versions:
            if v not in vers_locs:
                vers_locs[v] = d
        if not self.opts.silent :
            for v in SortVersions(vers_locs.keys()):
                output += 'echo %s in %s\n' % (v,vers_locs[v])
        self._write_script(output)

    def _ask_version(self, versions):
        # ask for a version
        versions = SortVersions([ v[1] for v in versions ])
        # string representations, to handle correctly projects without version
        str_versions = [ str(v) for v in versions ]
        ans = None
        while not ans:
            self._logger.write(self._logger.ALWAYS,
                               "Please enter your choice (%s q[uit] [%s]): "%
                               (" ".join(str_versions),str_versions[-1]))
            #self._always("Please enter your choice (%s q[uit] [%s]): "%
            #             (" ".join(versions),versions[-1]))
            ans = sys.stdin.readline().strip()
            if ans == '':
                ans = versions[-1]
            elif ans.lower() in [ 'q', 'quit' ]:
                return "quit"
            elif ans not in str_versions:
                self._always("Version '%s' not valid!"%ans)
                ans = None
        if ans:
            self._always("Trying version '%s'"%ans)
            # handle project without version
            if ans == 'None' and None in versions:
                ans = None
        return ans
    
    def _touch_project_logfiles(self):
        self._debug("----- _touch_project_logfiles() -----")
        if self.shell in [ 'csh', 'sh' ]:
            # I have to touch a file to tell the release manager which version of the project I'm using
            if 'LHCBHOME' in os.environ and 'USER' in os.environ \
                and self.project_info.version \
                and lhcb_style_version.match(self.project_info.version): # I do not want to record non-standard or no versions
                dirname = os.path.join(os.environ['LHCBHOME'],'project','logfiles')
                if os.path.isdir(dirname):
                    return 'touch %s/%s_%s_%s\n'%(dirname,
                                                  self.project_info.name.upper(),
                                                  self.project_info.version,
                                                  os.environ["USER"])
        return ""

    def _prepare_tmp_local_project(self):
        # prepare temporary local project directory
        self.tmp_root = TempDir()
        
        tmp_dir = os.path.join(self.tmp_root,"v23") # add a fake version directory to please CMT
        os.mkdir(tmp_dir)
        
        self._debug("Using temporary directory '%s'"%tmp_dir)
        
        # prepare temporary requirements file
        req = open(os.path.join(tmp_dir,"requirements"),"w")
        if not self.external_only and self.project_info.sys:
            # if we were not asked only for external and the Sys package exists,
            # we use it
            req.write("use %s" % self.project_info.sys)
            # this is to handle projects without version
            if self.project_info.version:
                req.write(" %s\n" % self.project_info.version)
            else:
                req.write(" v*\n")
        
        # add user specified tags
        for t in self.tag_add:
            req.write("apply_tag %s\n"%t)

        # prepare use statementes for externals (with default version)
        # version override have to happen later
        for e in self.args + self.ext_versions.keys() + self.site_externals:
            # @todo:I do not like too much this few lines
            if e != 'CASTOR' or not self.disable_CASTOR: # skip CASTOR
                req.write("use %s v* LCG_Interfaces\n"%e)
        
        # prepare use statementes for user packages
        use_hats = { 'XmlDDDB':'Det',
                     'XmlConditions':'Det',
                     'DecFiles':'Gen',
                     'SQLDDDB':'Det' }
        use_rexp = re.compile("^(?:([^/]*)/)?([^/ .]*)[ .]*([^ ]+)? *$")
        for u in self.use:
            m = use_rexp.match(u)
            if m: # handle the case of "use" string in the format "[Hat/]Package [version]"
                hat,pack,ver = m.groups()
                if ver is None:
                    ver = 'v*'
                if hat is None:
                    if pack in use_hats:
                        hat = use_hats[pack]
                    else:
                        hat = ''
                req.write("use %s %s %s\n"%(pack,ver,hat))
            else: # for any other case, just use the provided string
                req.write("use %s\n"%u)
        
        # set config versions
        # (the override of the versions have to be done after the package:
        #  some packages may change the default version, but the command line
        #  must win)
        for e in self.ext_versions:
            req.write('macro %s_config_version "%s"\n'%(e,self.ext_versions[e]))

        # use external location relative to the LCG Interfaces for the local installation

        req.write('macro LCG_home "$(LCG_home)" LOCAL&Unix "$(LCG_SETTINGSROOT)/../../../.." LOCAL&Unix "$(LCG_SETTINGSROOT)\\..\\..\\..\\.."\n')

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
        
    def _gen_setup(self, root_dir, env = os.environ):
        """
        Generate the setup script.
        Returns (script,error) where script is a string and error a list.
        If script is empty, you should find the reason in error.
        """
        self._debug("----- gen_setup() -----")
        
        if 'CMTPATH' not in os.environ and 'CMTPROJECTPATH' not in os.environ :
            # it does not make sense to go on if the CMTPATH is not set
            raise SetupProjectError('neither CMTPATH nor CMTPROJECTPATH are set')
        
        orig_dir = os.getcwd()
        
        if self.context_path:
            env['CMTUSERCONTEXT'] = self.context_path
        
        os.chdir(root_dir)
        
        # check CMT
        out = self.cmt.version()
        if "command not found" in out:
            raise SetupProjectError('cmt is not in your PATH')
        else:
            self._debug("----- using CMT %s -----" % out.strip())
        
        # check if the project works
        out = self.cmt.check("configuration")
        if out and not self.ignore_missing: # non empty output means error
            raise SetupProjectError(out)
        
        script = self.cmt.setup("-" + self.shell)
        os.chdir(orig_dir)
            
        #parse the output
        new_env = TemporaryEnvironment(env)
        # this sets the environment end return the line it cannot understand
        script = ShellParser[self.shell](script, new_env)
        
        # remove the temporary directory from the paths
        for v in ["PATH", "PYTHONPATH", "LD_LIBRARY_PATH"]:
            if v in new_env:
                new_env[v] = os.pathsep.join([ d
                                               for d in new_env[v].split(os.pathsep)
                                               if root_dir not in d ])
        # FIXME: I should look for all the variables pointing to the temporary directory
        
        # remove CMTPATH if not explicitely requested
        if not self.set_CMTPATH and "CMTPATH" in new_env:
            del new_env["CMTPATH"]
        
        if self.context_path:
            if not self.opts.silent :
                output_lines.append("echo Using CMTUSERCONTEXT = '%s'"%self.context_path)
            # unset CMTUSERCONTEXT in case of future calls
            del new_env['CMTUSERCONTEXT']
        
        lines = []
        errors = []
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

    def parse_args(self,args = sys.argv[1:]):
        self.opts,self.args = self.parser.parse_args(args=args)
    
    def prepare(self,args = sys.argv[1:]):
        """
        Collect all the informations needed to set up the environment, like
        project name and version, paths, etc.
        """
        # Process commmand line options
        self.parse_args(args=args)
        
        # set level of log messages
        if not self.opts.silent :
            self._logger.level = self.loglevel
        else :
            self._logger.level = self._logger.ERROR
        log.level = self.loglevel
        
        self._debug("----- main() -----")
        
        self._verbose("SetupProject %s"%self.parser.get_version())
        
        #------------- get project name
        if len(self.args) < 1:
            self._error("You have to specify a project")
            return 1
        else:
            self.project_name = self.args.pop(0)
        
        #------------- sanity check: the shell type has to be specified
        if not self.shell:
            self._error("Internal error: shell type not specified")
            return 1
        
        #------------- prepare search_path
        self.search_path = []
        if self.dev:
            self.search_path += self.dev_dirs
        # default locations
        for v in ["CMTPROJECTPATH", "LHCBPROJECTPATH"]:
            if v in os.environ:
                self.search_path += os.environ[v].split(os.pathsep)
        # remove duplicates
        self.search_path = uniq(self.search_path)
        
        #------------- set user area
        self.user_area = os.environ.get('User_release_area', None)
                    
        #------------- discover all project versions
        # debug printout: print project, search path and, optionally, user area
        self._debug("Look for all versions of '%s' in %s" % (self.project_name, self.search_path)
                    + ((self.user_area and (" with user area in '%s'" % self.user_area)) or ""))
        versions = FindProjectVersions(self.project_name, self.search_path, self.user_area)
        
        if not versions:
            self._error("Cannot find project '%s'" % self.project_name)
            return 1
        
        # Now I can print the version if I was asked to do it
        if self.list_versions:
            self._print_versions(versions)
            return 0

        #------------- project version
        if self.args:
            if self.args[0] in [ v[1] for v in versions ]:
                self.project_version = self.args.pop(0)
            elif re.match('v[0-9]*r[0-9p]*|HEAD',self.args[0]):
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
                                            versions = versions)
        if not self.project_info:
            # we should never get here
            self._error("PANIC: Cannot find version '%s' of %s after initial check" % (self.project_name, self.project_version))
            return 1
        
        # runtime projects
        self.runtime_projects = []
        for p,v in self.opts.runtime_projects:
            vv = FindProjectVersions(p, self.search_path, self.user_area)
            if not vv:
                self._error("Cannot find project '%s'" % p)
                return 1
            pi = makeProjectInfo(version = v,
                                 versions = vv)
            if not pi:
                self._error("Cannot find version '%s' of %s. Try with --list-versions." % (v, p))
                return 1                
            self.runtime_projects.append(pi)
        
        # overriding projects
        self.overriding_projects = []
        for p,v in self.opts.overriding_projects:
            vv = FindProjectVersions(p, self.search_path, self.user_area)
            if not vv:
                self._error("Cannot find project '%s'" % p)
                return 1
            pi = makeProjectInfo(version = v,
                                 versions = vv)
            if not pi:
                self._error("Cannot find version '%s' of %s. Try with --list-versions." % (v, p))
                return 1                
            self.overriding_projects.append(pi)
        
        # use LHCbGrid
        if self.use_grid:
            auto_override_projects.append( ("LHCbGrid", ["LHCbGridSys"]) )
        
        # auto-override projects
        if self.auto_override:
            for p, pkgs  in auto_override_projects:
                vv = FindProjectVersions(p, self.search_path, self.user_area)
                if vv:
                    self.overriding_projects.insert(0, makeProjectInfo(versions = vv))
                    self.use += pkgs
        
        for p in self.overriding_projects + [self.project_info] + self.runtime_projects :
            self._verbose("Project %s %s uses %s policy"%(p.name,
                                                          p.version,
                                                          p.policy))
        return 0
        
    def main(self,args = sys.argv[1:]):
        # Initialization from arguments
        rc = self.prepare(args)
        if rc or self.list_versions: # No need to go on if --list-versions or error
            return rc
        
        # Temporary enviroment
        env = TemporaryEnvironment()
        
        # Prepare the build-time environment
        self.set_build_env(env)
        # We usually unset CMTPATH, but we need to remember it for old style
        # projects
        CMTPATH = env.get("CMTPATH","")
        
        script = "" # things we need to append to the setup script (like aliases)
        messages = [] # lines to print (feedback)
        if not self.build_env: # this part is needed only if we do not ask for build only env
            self._always("Configuring %s" % self.project_info)
            tmp_dir = self._prepare_tmp_local_project()
            try:
                (script,err) = self._gen_setup(tmp_dir,env)
                if err and self.ignore_missing:
                    self._always("WARNING - ignoring missing packages:")
                    self._always("\n".join(err))
            except SetupProjectError, x:
                self._error(str(x))
                if 'package CASTOR' in str(x):
                    self._error("Tip: try adding --disable-CASTOR to the options")
                self._error("Could not produce the environment, check the arguments")
                return 1
            # Feedback
            if 'CMTPROJECTPATH' in env:
                messages.append("Using CMTPROJECTPATH = '%(CMTPROJECTPATH)s'" % env)
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
                lines[0] = '(taken from %s)' % lines[0]
            messages += lines
            
            # FIXME: Hack to hide the fact that old projects were not setting the PATH for the executable
            if self.project_info.policy == 'old':
                varname = self.project_info.name.upper() + "ROOT"
                if varname in env:
                    exedir = os.path.join(env[varname], env["CMTCONFIG"])
                    if os.path.isdir(exedir):
                        # it make sense to add it only if it exists
                        messages.append('Appending %s to the path.' % exedir)
                        env["PATH"] = os.pathsep.join([env["PATH"], exedir])
                
        else:
            tmps = self.project_info.name
            if self.project_info.version: tmps += " %s" % self.project_info.version
            messages.append('Build-time environment for %s ready.' % tmps)
            # create the user project if User_release_area is defined, it is a
            # project with InstallArea and the directory doesn't exist yet.
            if self.user_area:
                user_proj_name = "%s_%s" % (self.project_info.name, self.project_info.version)
                user_proj_dir = os.path.join(self.user_area, user_proj_name)
                if self.project_info.policy != 'old':
                    if not os.path.exists(user_proj_dir):
                        cur_dir = os.getcwd()
                        os.chdir(self.user_area)
                        self.cmt.create_project([user_proj_name,"-use=%s" % self.project_info.realName.replace(os.path.sep,":")])
                        os.chdir(cur_dir)
                        if os.path.isdir(user_proj_dir):
                            messages.append('Created user project in %s' % user_proj_dir)
                        else:
                            messages.append('Cannot create user project in %s' % user_proj_dir)
                        
                if os.path.isdir(user_proj_dir):
                    # Let's enter the user project directory
                    script += "cd %s\n" % user_proj_dir
                    messages.append("Current directory is '%s'." % user_proj_dir)
                
            if 'CMTPROJECTPATH' in env:
                messages.append("Using CMTPROJECTPATH = '%(CMTPROJECTPATH)s'" % env)
            else:
                messages.append("Using CMTPATH = '%s'" % CMTPATH)
            
        # normalize relative path introduced for the location the externals relatively
        # to the LCGCMT project
        path2normalize = ["PYTHONPATH", "PATH", "LD_LIBRARY_PATH", "ROOTSYS"]
        for p in path2normalize :
            if p in env.keys() :
                pthlist = env[p].split(os.pathsep)
                newlist = []
                for l in pthlist :
                    newpath = os.path.normpath(l)
                    if os.path.exists(newpath) and l.find("..") != -1 :
                        newlist.append(newpath)
                    else : 
                        newlist.append(l)
                env[p] = os.pathsep.join(newlist)
                
        output_script = env.gen_script(self.shell)
        output_script += script
        if not self.opts.silent :
            for m in messages:
                output_script += 'echo "%s"\n' % m
        #I have to touch a file to tell the release manager which version of the project I'm using
        output_script += self._touch_project_logfiles()
        
        self._verbose("########## done ##########")
        
        # as very last step, generate the output script to be sourced by the wrapper
        self._write_script(output_script)
    
        ### completed successfully
        return 0

if __name__ == '__main__':
    sys.exit(SetupProject().main(sys.argv[1:]))

