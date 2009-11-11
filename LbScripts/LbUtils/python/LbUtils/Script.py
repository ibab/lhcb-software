# $Id: Script.py,v 1.11 2009-11-11 10:15:28 hmdegaud Exp $
from LbUtils.Option import Parser
from LbUtils.ConfigFile import addConfigFileOptions, readConfigFile
from LbUtils.ConfigFile import setConfigFileDefaultValues
from LbUtils.ConfigFile import setDefaultConfig
from LbUtils import Env
import logging

import sys
import os

class PlainScript:
    _version = "$Id: Script.py,v 1.11 2009-11-11 10:15:28 hmdegaud Exp $".replace("$","").replace("Id:","").strip()
    _description = ""
    def __init__(self, usage=None, version=None, parser=Parser, 
                 help_output=sys.stdout, description=None):
        """ constructor of the Script. provides default options parser 
        and logger """
        if version is None:
            version = self._version 
        if description is None :
            description = self._description
        self.parser = parser(usage=usage, version=version, 
                             help_output=help_output, description=description)
        self.env = Env.getDefaultEnv()
        self.log = logging.getLogger()
        self.options = None
        self.args = None
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
        self.defineOpts()
        self.parseOpts(args)
        return self.main()
        
class ConfigScript(PlainScript):
    def __init__(self, usage=None, version=None, parser=Parser, 
                 help_output=sys.stdout, description=None, use_config_file=False):
        """ constructor of the Script. provides default options parser 
        and logger """
        PlainScript.__init__(self, usage, version, parser, help_output, description)
        self.use_config_file = use_config_file
        self._config = None
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
            home_cfg = os.path.join(os.environ.get("HOME", "."), ".config")
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
                conf_name = o
                conf_value = None
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
    def run(self, args=sys.argv[1:]):
        """ main function to be called by the user """
        self.defineOpts()
        if self.use_config_file :
            self.readDefaultConfig()
        self.parseOpts(args)
        return self.main()


# backward compatibility for old clients
Script = PlainScript