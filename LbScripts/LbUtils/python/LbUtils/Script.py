# $Id$
from LbUtils.Option import Parser
from LbUtils.ConfigFile import addConfigFileOptions, readConfigFile
from LbUtils.ConfigFile import setConfigFileDefaultValues
from LbUtils.ConfigFile import setDefaultConfig
from LbUtils.Env import Environment, Aliases, getDefaultEnv

from tempfile import mkstemp
from optparse import OptionValueError

import logging
import sys
import os

class PlainScript:
    _version = "$Id$".replace("$","").replace("Id:","").strip()
    _description = ""
    def __init__(self, usage=None, version=None, parser=Parser,
                 help_output=sys.stdout, description=None):
        """ constructor of the Script. provides default options parser
        and logger """
        if version is None:
            if hasattr(self, "__version__"):
                version = self.__version__ #IGNORE:E1101
            else:
                version = self._version

        if description is None :
            if self._description:
                description = self._description
            else:
                description = self.__doc__
        if usage is None :
            if hasattr(self, "__usage__"):
                usage = self.__usage__ #IGNORE:E1101
        self.parser = parser(usage=usage, version=version,
                             help_output=help_output, description=description)
        self.env = getDefaultEnv()
        self.log = logging.getLogger()
        self.options = None
        self.args = None
        # Add the default options for profiling
        self.addProfileOpts()
        self.defineOpts()
    def defineOpts(self):
        """ User options -- has to be overridden """
        # self.parser.add_options(...)
        pass
    def main(self):
        """ User code place holder -- has to be overridden """
        return 0
    def parseOpts(self, args):
        """ do the parsing (behind the scene) """
        self.options, self.args = self.parser.parse_args(args)
    def run(self, args=sys.argv[1:]):
        """ main function to be called by the user """
        self.parseOpts(args)
        # Prepare the profiler instance if needed
        profiler = None
        if self.options.profile:
            try:
                # Check first if (faster) cProfile is available
                import cProfile
                profiler = cProfile.Profile()
            except ImportError:
                try:
                    # Otherwise use the default  profiling library
                    import profile
                    profiler = profile.Profile()
                except ImportError:
                    self.log.warning("Cannot import cProfile or profile: ignoring --profile")
        # if we managed to get the profiler instance, collect profiling stats,
        # and print them in a specially created StringIO buffer
        if profiler:
            # Actually running and profiling the call
            rc = profiler.runcall(self.main)
            # Using stringIO to print the profile results to a buffer instead of stdout/stderr
            import StringIO
            output = StringIO.StringIO()

            # adapted from Python standard profiler.py
            import pstats
            stdoutSwapped = False
            if sys.version_info < (2, 5):
                # hack to be able to print the statistics in the StringIO buffer on older Python
                # This should be ok, as at this point the profiled script has finished running
                stats = pstats.Stats(profiler)
                sys.stdout = output
                stdoutSwapped = True
            else:
                stats = pstats.Stats(profiler, stream=output)

            if self.options.profdump != None:
                # Dump the statistics to a file
                stats.dump_stats(self.options.profdump)
            else:
                # print the stats to the StringIO buffer
                stats.strip_dirs().sort_stats(self.options.profsortby).print_stats(self.options.profmaxlines)
                # Log the actual statistics
                self.log.warning(output.getvalue())

            # Now resetting stdout if it was swapped (on older version of python)
            if stdoutSwapped:
                sys.stdout = sys.__stdout__

        else:
            rc = self.main()

        return rc
    def addProfileOpts(self):
        """ Define the common profiling options """
        parser = self.parser
        parser.add_option("--profile", action="store_true", dest="profile", help="Enable profiling of the script")
        parser.add_option("--prof-maxlines", type="int", dest="profmaxlines", default=100,
                          help="Set the maximum lines of profiling to print out")
        parser.add_option("--prof-sortby", type="choice", choices=["stdname", "calls", "time", "cumulative"],
                          dest="profsortby", default="cumulative",
                          help="Specify the sort order for the statitics. Possibilities are: stdname, calls, time, cumulative")
        parser.add_option("--prof-dump", type="string", dest="profdump",
                          help="Dumps the statistics to the file file specified instead of displaying them")

class ConfigScript(PlainScript):
    _version = "$Id$".replace("$","").replace("Id:","").strip()
    _description = ""
    def __init__(self, usage=None, version=None, parser=Parser,
                 help_output=sys.stdout, description=None, use_config_file=False):
        """ constructor of the Script. provides default options parser
        and logger """
        PlainScript.__init__(self, usage, version, parser, help_output, description)
        self.use_config_file = use_config_file
        self._config = None
        if self.use_config_file :
            self.readDefaultConfig()
    def setConfigFile(self, config_file=None, config_dir=None,
                      config_name=None, config_ext=None):
        self.use_config_file = True
        if config_file :
            if config_dir :
                sys.exit("Cannot use both config file and config dir")
            if config_name :
                sys.exit("Cannot use both config file and config name")
            if config_ext :
                sys.exit("Cannot use both config file and config ext")
        else :
            if not config_dir :
                config_dir = []
            home_cfg = os.environ.get("XDG_CONFIG_HOME",
                                      os.path.join(os.environ.get("HOME", "."), ".config"))
            if home_cfg not in config_dir :
                config_dir.append(home_cfg)
            if not config_name :
                config_name = self.__class__.__name__
            if  not config_ext :
                config_ext = "conf"
        addConfigFileOptions(self.parser, config_file, config_dir, config_name, config_ext)
    def getConfigFile(self):
        if self.parser.defaults["config_file"] :
            cf = self.parser.defaults["config_file"]
        else :
            cf = os.path.join(self.parser.defaults["config_dir"][0],
                              self.parser.defaults["config_name"] + "." + self.parser.defaults["config_ext"])
        return cf
    def hasDefaultConfig(self):
        found = False
        if self.parser.defaults.get("config_file", None) :
            found = True
        else :
            if self.parser.defaults.get("config_dir", None) :
                found = True
            if self.parser.defaults.get("config_name", None) :
                found = True
            if self.parser.defaults.get("config_ext", None) :
                found = True
        return found
    def readDefaultConfig(self):
        """ read the hardcoded or guessed config file. This sets the defaults
            values for the parser. """
        if not self.hasDefaultConfig() :
            self.setConfigFile()
        self._config = readConfigFile(self.parser, self.getConfigFile())
    def parseConfigArgs(self, args):
        args_cpy = args[:]
        config_opts = ["--config-file", "--config-name", '--config-dir',
                       "--config-dir-append", "--config-dir-prepend",
                       "--config-ext"]
        config_file = self.parser.defaults["config_file"]
        config_name = self.parser.defaults["config_name"]
        config_dir  = self.parser.defaults["config_dir"]
        config_ext  = self.parser.defaults["config_ext"]
        for a in args_cpy :
            for o in config_opts :
#                conf_name = o
#                conf_value = None
                if a.startswith(o) :
                    if a == o :
                        pass
        setConfigFileDefaultValues(self.parser, config_file, config_dir, config_name, config_ext)
        if not self.parser.defaults["no_config_file"] :
            self.readDefaultConfig()
            setDefaultConfig(self.parser, self._config)
        return args_cpy
    def parseOpts(self, args):
        """ do the parsing (behind the scene) """
        if self.use_config_file :
            args = self.parseConfigArgs(args)
        PlainScript.parseOpts(self, args)

# backward compatibility for old clients
Script = PlainScript

def _check_output_options_cb(option, opt_str, value, parser):
    if opt_str == "--mktemp":
        if parser.values.output:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.mktemp = True
    elif opt_str == "--output":
        if parser.values.mktemp:
            raise OptionValueError("--mktemp cannot be used at the same time as --output")
        parser.values.output = value

class SourceScript(Script):
    _version = "$Id$".replace("$","").replace("Id:","").strip()
    _description = ""
    def __init__(self, usage=None, version=None, parser=Parser, description=None):
        Script.__init__(self, usage=usage, version=version, parser=parser,
                        help_output=sys.stderr, description=description)
        self.output_file = None
        self.output_name = None
        self._env = Environment()
        self._aliases = Aliases()
        self._extra = ""
        self.addSourceOpts()
    def _write_script(self, env):
        """ select the ouput stream according to the cmd line options """
        log = logging.getLogger()
        close_output = False
        if self.options.output:
            self.output_file = open(self.options.output, "w")
            self.output_name = self.options.output
            self.options.output = None # reset the option value to avoid to reuse it
            close_output = True
        elif self.options.mktemp:
            fd, outname = mkstemp()
            self.output_name = outname
            self.output_file = os.fdopen(fd, "w")
            print outname
            self.options.mktemp = None # reset the option value to avoid to reuse it
            close_output = True
        else :
            self.output_file = sys.stdout
            close_output = False
        # write the data
        if self.output_name :
            log.debug("Writing output to %s" % self.output_name)
        self.output_file.write(env)
        self.output_file.write("\n") # @todo: this may be avoided
        if close_output:
            self.output_file.close()
    def addSourceOpts(self):
        parser = self.parser
        parser.set_defaults(targetshell="csh")
        parser.add_option("--shell", action="store", type="choice", metavar="SHELL",
                          dest="targetshell",
                          choices=['csh', 'sh', 'bat'],
                          help="(internal) select the type of shell to use")
        parser.set_defaults(mktemp=False)
        parser.add_option("--mktemp", action="callback",
                          callback=_check_output_options_cb,
                          help="(internal) send the output to a temporary file and print on stdout the file name (like mktemp)")
        parser.set_defaults(output=None)
        parser.add_option("--output", action="callback", metavar="FILE",
                          type="string", callback=_check_output_options_cb,
                          help="(internal) output the command to set up the environment to the given file instead of stdout")
    def flush(self):
        opts = self.options
        self._write_script(self._env.gen_script(opts.targetshell)
                           + self._aliases.gen_script(opts.targetshell)
                           + self.extra())
    def Environment(self):
        return self._env
    def Aliases(self):
        return self._aliases
    def targetShell(self):
        return self.options.targetshell
    def addEcho(self, line):
        if line[-1] == "\n" :
            line = line[:-1]
        if sys.platform != "win32" :
            outline = "echo '%s'\n" % line
        else :
            outline = "echo %s\n" % line
        self._extra += outline
    def extra(self):
        return self._extra
    def addExtra(self, line):
        self._extra += line

