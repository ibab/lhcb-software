# $Id$
from ConfigParser import SafeConfigParser

import os

def cb_configFile(option, opt_str, value, parser):
    setattr(parser.values, option.dest, value)
    setattr(parser.values, "config_dir", None)
    setattr(parser.values, "config_name", None)

def cb_configDir(option, opt_str, value, parser):
    if not parser.values.config_dir is None:
        parser.values.config_dir = []
    if opt_str == "--config-dir" :
        parser.values.config_dir = value.split(os.pathsep)
    if opt_str == "--config-dir-prepend" :
        parser.values.config_dir = value.split(os.pathsep) + parser.values.config_dir
    if opt_str == "--config-dir-append" :
        parser.values.config_dir += value.split(os.pathsep)
    if opt_str == "--config-dir" :
        parser.values.config_dir = value.split(os.pathsep)
    parser.values.config_dir.append(value)
    parser.values.config_file = None

def cb_configName(option, opt_str, value, parser):
    parser.values.config_name = value
    parser.values.config_file = None

def cb_configExt(option, opt_str, value, parser):
    parser.values.config_name = value
    parser.values.config_file = None

def setConfigFileDefaultValues(parser, config_file, config_dir,
                               config_name, config_ext):
    parser.set_defaults(config_file=config_file)
    parser.set_defaults(config_dir=config_dir)
    parser.set_defaults(config_name=config_name)
    parser.set_defaults(config_ext=config_ext)

def addConfigFileOptions(parser, config_file, config_dir,
                         config_name, config_ext):
    grp = parser.add_option_group("Config")
#    parser.set_defaults(config_file=config_file)
    # @todo: exclude the usage of both options --config-file and --config-dir
    setConfigFileDefaultValues(parser, config_file, config_dir, config_name, config_ext)
    grp.add_option("--config-file",
                   action="callback",
                   callback=cb_configFile,
                   nargs=1,
                   dest="config_file",
                   help="Path to the config file [default %default]")
    grp.add_option("--config-dir",
                   action="callback",
                   callback=cb_configDir,
                   nargs=1,
                   dest="config_dir",
                   help="Set configuration directory [default %default]")
    grp.add_option("--config-dir-prepend",
                   action="callback",
                   callback=cb_configDir,
                   nargs=1,
                   dest="config_dir",
                   help="Append configuration directory")
    grp.add_option("--config-dir-append",
                   action="callback",
                   callback=cb_configDir,
                   nargs=1,
                   dest="config_dir",
                   help="Set configuration directory")
    grp.add_option("--config-name",
                   action="callback",
                   callback=cb_configName,
                   nargs=1,
                   dest="config_name",
                   help="Name for the configuration file [default %default]")
    grp.add_option("--config-ext",
                   action="callback",
                   callback=cb_configExt,
                   nargs=1,
                   dest="config_ext",
                   help="Name for the configuration file extension [default %default]")
    parser.set_defaults(no_config_file=False)
    grp.add_option("--no-config-file",
                   dest="no_config_file",
                   action="store_true",
                   help="Prevents the reading of the configuration files")
    parser.set_defaults(print_config=False)
    grp.add_option("--print-config",
                   dest="print_config",
                   action="store_true",
                   help="Prints the configuration and exits")

def readOptionConfig(group_title, option, config_parser):
    config = None
    if option.action == "store_true" or option.action=="store_false" :
        config = config_parser.getboolean(group_title, option.dest)
    elif option.dest :
        config = config_parser.get(group_title, option.dest)
    return config

def readGroupConfig(group, config_parser):
    config = {}
    for o in group.option_list :
        config[o.dest] = readOptionConfig(group.title, o, config_parser)
    return config

def readMainConfig(parser, config_parser):
    config = {}
    for o in parser.option_list:
        config[o.dest] = readOptionConfig("DEFAULT", o, config_parser)
    for o in config.keys() :
        if not o :
            del config[o]
    return config

def readConfigFile(parser, config_file=None):
    config_files = []
    if not config_file :
        config_files.append(parser.defaults.get("config_file", None))
    else :
        config_files.append(config_file)
    config = {}
    if config_files :
        cf = SafeConfigParser()
        good_files = cf.read(config_files)
        if good_files :
            config["DEFAULT"] = readMainConfig(parser, cf)
            for g in parser.option_groups :
                if g.title != "Config" and g.title in cf.sections() :
                    config[g.title] = readGroupConfig(g, cf)
    return config

def setDefaultConfig(parser, config):
    for g in config.keys() :
        for o in config[g].keys() :
            parser.defaults[o] = config[g][o]

