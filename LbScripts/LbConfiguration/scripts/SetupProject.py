#!/usr/bin/env python

import os, sys, tempfile, re, sys
from stat import S_ISDIR
import getopt

_cvs_id = "$Id: SetupProject.py,v 1.1 2008-05-16 09:17:40 hmdegaud Exp $"
        
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
    def __init__(self,out=sys.stderr):
        self.out = out
        self.level = 3
    def write(self,level,s):
        if level >= self.level:
            self.out.write(s)
            self.out.flush()
    def writeln(self,level,s):
        if level >= self.level:
            self.out.write(s)
            self.out.write("\n")
            self.out.flush()

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
        Revert all the changes done to the orignal environment.
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
        #print "Restoring the enviroment"
        self.restore()

    def gen_script(self,shell_type):
        """
        Generate a shell script to reproduce the changes in the environment.
        """
        shells = [ 'csh', 'sh', 'bat' ]
        if shell_type not in shells:
            raise RuntimeError("Shell type '%s' unknown. Available: %s"%(shell_type,shells))
        out = ""
        for key,value in self.old_values.items():
            if key not in self.env:
                # unset variable
                if shell_type == 'csh':
                    out += 'unsetenv %s\n'%key
                elif shell_type == 'sh':
                    out += 'unset %s\n'%key
                elif shell_type == 'bat':
                    out += 'set %s=\n'%key
            else:
                # set variable
                if shell_type == 'csh':
                    out += 'setenv %s "%s"\n'%(key,self.env[key])
                elif shell_type == 'sh':
                    out += 'export %s="%s"\n'%(key,self.env[key])
                elif shell_type == 'bat':
                    out += 'set %s=%s\n'%(key,self.env[key])
        return out

class TempDir:
    def __init__(self, keep = False):
        self.name = mkdtemp()
        self._keep = keep
        
    def __str__(self):
        return self.name
    
    def __del__(self):
        if self.name:
            removeall(self.name)
    
    def __getattr__(self,attr):
        return getattr(self.name,attr)

class ProjectInfoError (RuntimeError):
    def __init__(self,project,version,msg="Generic error"):
        self.project = project
        if version is not None:
            self.version = version
        else:
            self.version = 'unspecified'
        self.msg = msg
    def __str__(self):
        return "%s (project:%s, version:%s)"%(self.msg,self.project,self.version)

class ProjectInfo:
    """
    Class to get project related infos.
    """
    # @todo: use LHCB_config.py instead of scanning directories
    searchpath = []
    def __init__(self, project, version = None):
        self.name = project
        
        self.all_versions = ProjectInfo.get_all_versions(project)
        if not self.all_versions:
            raise ProjectInfoError(project,version,
                                   "Cannot find project in %s"%self.searchpath)
        
        if version is None:
            # by default use the most recent version
            self.version = self.all_versions[-1][0]
            self.project_dir = os.path.dirname(self.all_versions[-1][1])
        else:
            vers = filter(lambda v: v[0] == version, self.all_versions)
            if not vers:
                raise ProjectInfoError(project,version,
                                       "Version of the project not available")
            else:
                self.version = vers[0][0]
                self.project_dir = os.path.dirname(vers[0][1])
        
        # discover project policy
        release_area = os.path.dirname(self.project_dir)
        self._projectenv_cmt_dir = os.path.join(release_area,"%sEnv"%self.name,self.version,'cmt')
        if os.path.isdir(self._projectenv_cmt_dir):
            self.policy = 'old'
        else:
            self.policy = 'new'
        
        self.sys = self._projectsys()
        self.syscmtfullpath = os.path.join(self.project_dir,
                                           "%s_%s"%(self.name.upper(),self.version),
                                           self.sys,self.version,'cmt')
        
    def _projectsys(self):
        """
        Return the name of the package referencing all the packages belonging to a project.
        """
        if self.name == 'Gaudi':
            if self.policy == 'new':
                return self.name + "Release"
            else:
                return self.name
        else:
            return self.name + 'Sys'

    def get_all_versions(cls,project):
        """
        Find the available versions in the release area of the given project.
        Returns an ordered list of strings in the format 'vXrY[pZ]'.
        """ 
        # find all project directories in the searchpath
        PROJECT = project.upper()
        projectdirs = filter(os.path.isdir,
                             map(lambda d: os.path.join(d,PROJECT),cls.searchpath))
        
        # signature for a project version directory
        versions = {}
        if project != "LCGCMT":
            rexp = re.compile('%s_v([0-9]+)r([0-9]+)(p([0-9]+))?'%PROJECT)
        else:
            rexp = re.compile('%s_([0-9]+)([a-z])?()()'%PROJECT)
        
        for pd in projectdirs:
            for l in os.listdir(pd):
                m = rexp.match(l)
                if m :
                    v,r,dummy,p = m.groups()
                    v = int(v)
                    if project != "LCGCMT":
                        r = int(r)
                    if p:
                        p = int(p)
                    if (v,r,p) not in versions:
                        versions[(v,r,p)] = os.path.join(pd,l)
        verkeys = versions.keys()
        verkeys.sort()
        result = []
        for l in verkeys:
            if project != "LCGCMT":
                if not l[2]:
                    result.append(('v%dr%d'%(l[0],l[1]),versions[l]))
                else:
                    result.append(('v%dr%dp%d'%l,versions[l]))
            else:
                if not l[1]:
                    result.append(('%d'%l[0],versions[l]))
                else:
                    result.append(('%d%s'%(l[0],l[1]),versions[l]))
        return result
    get_all_versions = classmethod(get_all_versions)

    def set_build_env(self,env = os.environ, need_user_area = False):
        log.writeln(log.DEBUG,"----- set_build_info() -----")
        if 'User_release_area' in env:
            log.writeln(log.DEBUG,"----- User_release_area is '%s' -----"%env['User_release_area'])
        log.writeln(log.DEBUG,"----- need_user_area is '%s' -----"%need_user_area)
        if self.policy =='old':
            # old style project
            olddir = os.getcwd()
            os.chdir(self._projectenv_cmt_dir)
            if 'CMTPROJECTPATH' in env:
                log.writeln(log.DEBUG,"----- unsetenv CMTPROJECTPATH -----")
                del env['CMTPROJECTPATH']
            env['CMTPATH'] = os.popen("cmt show set_value CMTPATH").readline().strip()
            os.chdir(olddir)
            if 'User_release_area' in env:
                if env['CMTPATH'].find(env['User_release_area']) < 0:
                    env['CMTPATH'] =  os.pathsep.join([ env['User_release_area'],
                                                        env['CMTPATH'] ])
            log.writeln(log.DEBUG,"----- CMTPATH set to '%s' -----"%env['CMTPATH'])
            
        else:
            # new style project
            
            # prepare the basic CMTPROJECTPATH
            env['CMTPROJECTPATH'] = os.pathsep.join(self.searchpath)
            
            # check for user release area
            if 'User_release_area' in env and need_user_area:
                if env['CMTPROJECTPATH'].find(env['User_release_area']) < 0:
                    # since we need it ant it is not there, I add it
                    env['CMTPROJECTPATH'] = os.pathsep.join([ env['User_release_area'],
                                                              env['CMTPROJECTPATH'] ])
            log.writeln(log.DEBUG,"----- CMTPROJECTPATH set to '%s' -----"%env['CMTPROJECTPATH'])
            
            # unset CMTPATH if present
            if 'CMTPATH' in env:
                log.writeln(log.DEBUG,"----- unsetenv CMTPATH -----")
                del env['CMTPATH']
# set the default searchpath
for v in ["CMTPROJECTPATH", "LHCBPROJECTPATH"]:
    if v in os.environ:
        ProjectInfo.searchpath = os.environ[v].split(os.pathsep)
        break

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

########################################################################
# Main class
########################################################################
class SetupProject:
    def __init__(self):
        self._logger = Logger()
        self.cmt = CMT()
        
        self.opts = None
        self.args = None
        
        self.project_name = None
        self.project_version = None
        self.project_info = None
        
        self.project_in_user_area = False
        self.user_proj_name = ""
        
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
            parser.values.dev_dir = value
        parser.add_option("--dev", action="store_true",
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

        if "LHCBDEV" in os.environ:
            DEVDIR = os.environ["LHCBDEV"]
        else:
            DEVDIR = None 
        parser.set_defaults(output=None,
                            mktemp=False,
                            loglevel = 3,
                            disable_CASTOR=False,
                            dev_dir=DEVDIR,
                            ext_versions = {},
                            use = [],
                            tag_add = [],
                            set_CMTPATH = False,
                            runtime_projects = []
                            )
        
        if 'CMTSITE' in os.environ and \
            os.environ["CMTSITE"] in self.default_externals:
            parser.set_defaults(site_externals = self.default_externals[os.environ["CMTSITE"]])
        else:
            parser.set_defaults(site_externals = self.default_externals['none'])
        
        return parser
    
    def _print_versions(self):
        versions = ProjectInfo.get_all_versions(self.project_name)
        output = ''
        for v,d in versions:
            output += 'echo %s\n'%v
        if not output:
            output = "echo Cannot find project %s\n"%self.project_name
        self._write_script(output)

    def _ask_version(self):
        # ask for a version
        versions = map(lambda x: x[0],
                       ProjectInfo.get_all_versions(self.project_name))
        if not versions:
            self._always("Cannot find project %s"%self.project_name)
            return "quit"
        ans = None
        while not ans:
            self._logger.write(self._logger.ALWAYS,
                               "Please enter your choice (%s q[uit] [%s]): "%
                               (" ".join(versions),versions[-1]))
            #self._always("Please enter your choice (%s q[uit] [%s]): "%
            #             (" ".join(versions),versions[-1]))
            ans = sys.stdin.readline().strip()
            if ans == '':
                ans = versions[-1]
            elif ans.lower() in [ 'q', 'quit' ]:
                return "quit"
            elif ans not in versions:
                self._always("Version '%s' not valid!"%ans)
                ans = None
        if ans:
            self._always("Trying version '%s'"%ans)
        return ans
    
    def _touch_project_logfiles(self):
        self._debug("----- _touch_project_logfiles() -----")
        if self.shell in [ 'csh', 'sh' ]:
            # I have to touch a file to tell the release manager which version of the project I'm using
            if 'LHCBHOME' in os.environ and 'USER' in os.environ:
                dirname = os.path.join(os.environ['LHCBHOME'],'project','logfiles')
                if os.path.isdir(dirname):
                    return '\ntouch %s/%s_%s_${USER}\n'%(dirname,
                                                         self.project_info.name.upper(),
                                                         self.project_info.version)
        return ""

    def _prepare_tmp_local_project(self):
        # prepare temporary local project directory
        self.tmp_root = TempDir()
        
        tmp_dir = os.path.join(self.tmp_root,"v23") # add a fake version directory to please CMT
        os.mkdir(tmp_dir)
        
        self._debug("Using temporary directory '%s'"%tmp_dir)
        
        # prepare temporary requirements file
        req = open(os.path.join(tmp_dir,"requirements"),"w")
        if not self.external_only:
            # if we want only the externals, we do not put the project
            req.write("use %s %s\n"%(self.project_info.sys,
                                     self.project_info.version))
        
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
        use_hats = { 'XmlDDDB':'Det', 'XmlConditions':'Det', 'DecFiles':'Gen' }
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
        
        # set native versions
        # (the override of the versions have to be done after the package:
        #  some packages may change the default version, but the command line
        #  must win)
        for e in self.ext_versions:
            req.write('macro %s_native_version "%s"\n'%(e,self.ext_versions[e]))
        req.flush()
        del req

        # prepare temporary project file
        os.mkdir(os.path.join(tmp_dir,"cmt"))
        prj = open(os.path.join(tmp_dir,"cmt","project.cmt"),"w")
        if self.project_in_user_area:
            # We do not need to add the released project to the list of uses
            # because it is implicit in the usage of the user project (which
            # may override everything)
            prj.write("use %s\n"%self.user_proj_name)
        else:
            prj.write("use %s %s_%s\n"%(self.project_info.name.upper(),
                                        self.project_info.name.upper(),
                                        self.project_info.version))
        
        # Add the runtime extra projects to the list of used projects
        for p_info in self.runtime_projects:
            p = p_info.name.upper()
            prj.write("use %s %s_%s\n"%(p,p,p_info.version))
        
        prj.flush()
        del prj
        
        if self._logger.level <= 2 :
            self._verbose("########## requirements ##########")
            self._verbose(open(os.path.join(tmp_dir,"requirements")).read())
            self._verbose("########## project.cmt ##########")
            self._verbose(open(os.path.join(tmp_dir,"cmt","project.cmt")).read())
        
        return tmp_dir

    def _check_user_area(self):
        self._debug("----- _check_user_area -----")
        self.user_proj_name = "%s_%s"%(self.project_info.name,
                                       self.project_info.version)
        self._debug("user_proj_name = %s"%self.user_proj_name)
        if 'User_release_area' in os.environ:
            self._debug("User_release_area = %s"%os.environ['User_release_area'])
            self.project_in_user_area = os.path.isdir(os.path.join(os.environ['User_release_area'],
                                                                   self.user_proj_name))
            self._debug("project_in_user_area = %s"%self.project_in_user_area)
        else:
            self._debug("User_release_area undefined")
            self.project_in_user_area = False
        
    def _gen_setup(self,root_dir,env = os.environ):
        """
        Generate the setup script.
        Returns (script,error) where script is a string and error a list.
        If script is empty, you should find the reason in error.
        """
        self._debug("----- gen_setup() -----")
        
        if 'CMTPATH' not in os.environ and 'CMTPROJECTPATH' not in os.environ :
            # it does not make sense to go on if the CMTPATH is not set
            return ([],['neither CMTPATH nor CMTPROJECTPATH are set'])
        
        orig_dir = os.getcwd()
        
        if self.context_path:
            env['CMTUSERCONTEXT'] = self.context_path
        
        os.chdir(root_dir)
        lines = self.cmt.setup("-"+self.shell).split('\n')
        os.chdir(orig_dir)
        #search for errors
        output_lines = []
        error = []
        if os.sep != '\\':
            set_path_re = re.compile('^(.*PATH.*)%(path)s[^%(sep)s]*%(sep)s(.*)$'%{'path':root_dir,'sep':os.pathsep})
        else:
            set_path_re = re.compile(('^(.*PATH=)(?:%(path)s[^%(sep)s]*%(sep)s)+(.*)$'%{'path':root_dir,'sep':os.pathsep})
                                     .replace(os.sep,'\\\\'))
        for l in lines:
            if (l.find('Warning') >= 0):
                error.append(l.strip())
                continue
            elif (not self.set_CMTPATH and l.find('CMTPATH') >= 0): # CMT is setting internally the CMTPATH, I ignore it
                continue
            elif (l.find('command not found') >= 0) or (l.find('not recognized') >= 0):
                raise RuntimeError("cmt is not in your PATH")
            else:
                # strip the temporary directory from paths
                m = set_path_re.match(l)
                while m:
                    l = m.group(1)+m.group(2)+"\n"
                    m = set_path_re.match(l)
                    
            output_lines.append(l.strip())
    
        if self.context_path:
            output_lines.append("echo Using CMTUSERCONTEXT = '%s'"%self.context_path)
            # unset CMTUSERCONTEXT in case of future calls
            del env['CMTUSERCONTEXT']
                    
        if (error):
            log.writeln(log.DEBUG,"----- gen_setup(): error != [] -----")
        return ('\n'.join(output_lines), error)

    def parse_args(self,args = sys.argv[1:]):
        self.opts,self.args = self.parser.parse_args(args=args)
        
    def main(self,args = sys.argv[1:]):
    
        # Process commmand line options
        self.parse_args(args=args)
        
        # set level of log messages
        self._logger.level = self.loglevel
        log.level = self.loglevel
        
        self._debug("----- main() -----")
        
        self._verbose("SetupProject %s"%self.parser.get_version())

        # get project name
        if len(self.args) < 1:
            self._error("You have to specify a project")
            return 1
        else:
            self.project_name = self.args.pop(0)
        
        # get project version if specified
        if self.args and re.match('v[0-9]*r[0-9p]*',self.args[0]):
            self.project_version = self.args.pop(0)
        
        # check is the shell type has been specified
        if not self.shell:
            self._error("Internal error: shell type not specified")
            return 1
        
        # prepare searchpath
        if self.dev:
            if self.dev_dir:
                ProjectInfo.searchpath.insert(0,self.dev_dir)
            else:
                self._warning("Option --dev used, but LHCBDEV is not defined (so ignored)")
        
        ### Quick actions
        try:
            
            if self.list_versions:
                self._print_versions()
                return 0
            
            if self.ask:
                v = self._ask_version()
                if v == "quit":
                    self._always("Quit")
                    return 1
                self.project_version = v
            
            ### Initialize the Projectinfo object
            self.project_info = ProjectInfo(self.project_name,self.project_version)
            rt_projs = []
            for p,v in self.runtime_projects:
                rt_projs.append(ProjectInfo(p,v))
            self.runtime_projects = rt_projs
            
        except ProjectInfoError, x:
            if x.msg == "Version of the project not available":
                self._error("ERROR - %s. Try with --list-versions."%x)
            else:
                self._error("ERROR - %s"%x)
            return 1
        
        self._verbose("Project %s %s uses %s policy"%(self.project_info.name,
                                                      self.project_info.version,
                                                      self.project_info.policy))

        self._check_user_area() # check if the user has a local project with the name specified
        
        ### Prepare the enviroment
        env = TemporaryEnvironment()
        
        ## --- Build environment
        
        self.project_info.set_build_env(env,self.project_in_user_area)
        output = env.gen_script(self.shell)
        # I have to touch a file to tell the release manager which version of the project I'm using
        output += self._touch_project_logfiles()
        
        if not self.build_env: # this part is needed only if we do not ask for build only env

            tmp_dir = self._prepare_tmp_local_project()
            
            (setup_out,err) = self._gen_setup(tmp_dir,env)
            
            if err and self.ignore_missing:
                self._always("WARNING - ignoring missing packages:")
                self._always("\n".join(err))
            elif err:
                output = ""
                for e in err:
                    if e.find('package CASTOR') >= 0:
                        err.append("\nTip: try adding --disable-CASTOR to the options")
                        break
                self._error('\n'.join(err))
                self._error("Could not produce the environment, check the arguments")
                return 1
            
            output += setup_out
            
            if 'CMTPROJECTPATH' in env:
                output += "\necho Using CMTPROJECTPATH = '%(CMTPROJECTPATH)s'"%env
            else:
                output += "\necho Using CMTPATH = '%(CMTPATH)s'"%env

            output += '\necho Environment for %s %s ready.'%(self.project_info.name,
                                                             self.project_info.version)
            if not self.runtime_projects:
                output += '\necho "(taken from %s)"'%(self.project_info.project_dir)
            else:
                output += '\necho "(%s %s from %s'%(self.project_info.name,
                                                     self.project_info.version,
                                                     self.project_info.project_dir)
                for pi in self.runtime_projects:
                    output += ',"\necho " %s %s from %s'%(pi.name,
                                                       pi.version,
                                                       pi.project_dir)
                output += ')"'

        else:
            output += '\necho Build-time environment for %s %s ready.'%(project.name,project.version)
        
        self._verbose("########## done ##########")
        
        # as very last step, generate the output script to be sourced by the wrapper
        self._write_script(output)
    
        ### completed successfully
        return 0

if __name__ == '__main__':
    sys.exit(SetupProject().main(sys.argv[1:]))

