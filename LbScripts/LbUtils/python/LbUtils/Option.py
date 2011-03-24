# $Id$

from optparse import OptionParser, Option, OptionContainer, OptParseError
from LbUtils import Log, Env
import os, sys
import logging
import types
from gettext import gettext


NO_DEFAULT = ("NO", "DEFAULT")

class FallBackOption(Option):
    ATTRS = ['action',
             'type',
             'dest',
             'default',
             'nargs',
             'const',
             'choices',
             'callback',
             'callback_args',
             'callback_kwargs',
             'help',
             'metavar',
             'fallback_env',
             'fallback_conf',
             'mandatory']
    ACTIONS = ("store",
               "store_const",
               "store_true",
               "store_false",
               "append",
               "append_const",
               "count",
               "callback",
               "help",
               "longhelp",
               "version")

    def take_action(self, action, dest, opt, value, values, parser):
        if action == "longhelp" :
            parser.print_longhelp()
            parser.exit()
        else :
            Option.take_action(self, action, dest, opt, value, values, parser)


class CommandError(OptParseError):
    """
    Raised if an Command instance is created with invalid or
    inconsistent arguments.
    """

    def __init__(self, msg, command):
        self.msg = msg
        self.command_id = str(command)

    def __str__(self):
        if self.command_id:
            return "command %s: %s" % (self.command_id, self.msg)
        else:
            return self.msg



class Command:
    CHECK_METHODS = None
    ATTRS = []
    def __init__(self, *cmds, **attrs):
        self.parser = Parser()
        self._cmds = []
        self._set_cmd_strings(cmds)
        self._set_attrs(attrs)
        for checker in self.CHECK_METHODS:
            checker(self)
    def _set_cmd_strings(self, cmds):
        for cmd in cmds:
            if cmd[0] == "-" :
                raise CommandError(
                    "invalid command string %r: "
                    "has to start with a non-dash char" % cmd, self)
            else:
                self._cmds.append(cmd)
    def _set_attrs(self, attrs):
        for attr in self.ATTRS:
            if attrs.has_key(attr):
                setattr(self, attr, attrs[attr])
                del attrs[attr]
            else:
                if attr == 'default':
                    setattr(self, attr, NO_DEFAULT)
                else:
                    setattr(self, attr, None)
        if attrs:
            attrs = attrs.keys()
            attrs.sort()
            raise CommandError(
                "invalid keyword arguments: %s" % ", ".join(attrs),
                self)

class Parser(OptionParser):
    def __init__(self, *args, **kwargs):
        self.help_output = sys.stdout
        if kwargs.has_key("help_output") :
            self.help_output = kwargs["help_output"]
            del kwargs["help_output"]
        kwargs["option_class"] = FallBackOption
        OptionParser.__init__(self, *args, **kwargs) #IGNORE:W0142
        self._create_command_list()
        Log.addDefaultLogger(self)
        Env.addEnvironment(self)
    def _add_help_option(self):
        OptionParser._add_help_option(self)
        self.add_option( "--long-help",
                        action="longhelp",
                        help=gettext("show the complete help message and exit"))
    def add_option(self, *args, **kwargs):
        if kwargs.has_key("fallback_env") :
            kwargs["help"] += "\nThe fallback environment variable is set to %s." % kwargs["fallback_env"]
        if kwargs.has_key("fallback_conf") :
            kwargs["help"] += "\nThe fallback configuration is set to %s." % kwargs["fallback_conf"]
        OptionParser.add_option(self, *args, **kwargs) #IGNORE:W0142
    def check_values(self, values, args):
        log = logging.getLogger()
        for opt in self._get_all_options():
            fb_env = getattr(opt,"fallback_env")
            fb_conf = getattr(opt,"fallback_conf")
            fb_mand = getattr(opt, "mandatory")
            if fb_env or fb_conf :
                dest = getattr(opt, "dest")
                if not getattr(values, dest) :
                    if fb_env and os.environ.has_key(fb_env):
                        setattr(opt, "dest", os.environ[fb_env])
                        setattr(values, dest, os.environ[fb_env])
                        log.warning("using environment variable %s for %s" % (fb_env, dest))
                        log.info("%s is set to %s" % (dest, os.environ[fb_env]) )
                    elif fb_conf :
                        setattr(opt, "dest", fb_conf)
                        setattr(values, dest, fb_conf)
                        log.warning("using configuration fallback for %s: %s" % (dest, fb_conf))
                        log.info("%s is set to %s" % (dest, fb_conf) )
                    else :
                        if fb_mand :
                            log.error("no value for %s" % dest)
        return (values, args)

    def _create_command_list(self):
        self.command_list = []
        self._create_command_mappings()
    def _create_command_mappings(self):
        self._cmd = {}             # long option -> Option instance
    def add_command(self, *args, **kwargs):
        if type(args[0]) is types.StringType:
            command = Command(*args, **kwargs)
        elif len(args) == 1 and not kwargs:
            command = args[0]
            if not isinstance(command, Command):
                raise TypeError, "not an Command instance: %r" % command
        else:
            raise TypeError, "invalid arguments"
        self.command_list.append(command)
        command.container = self
        for cmd in command._cmds:
            self._cmd[cmd] = command
        return command
    def add_commands(self, command_list):
        for command in command_list:
            self.add_command(command)
    def get_command(self, cmd_str):
        return self._cmd.get(cmd_str)
    def has_command(self, cmd_str):
        return self._cmd.has_key(cmd_str)
    def remove_command(self, cmd_str):
        command = self._cmd.get(cmd_str)
        if command is None:
            raise ValueError("no such command %r" % cmd_str)
        for cmd in command._cmds:
            del self._cmd[cmd]
        command.container.command_list.remove(command)


    def format_option_help(self, formatter=None):
        if formatter is None:
            formatter = self.formatter
        formatter.store_option_strings(self)
        result = []
        result.append(formatter.format_heading(gettext("Options")))
        formatter.indent()
        if self.option_list:
            result.append(OptionContainer.format_option_help(self, formatter))
            result.append("\n")
        for group in self.option_groups:
            if group.title != "Logging" and group.title != "Environment" and group.title != "Config":
                result.append(group.format_help(formatter))
                result.append("\n")
        formatter.dedent()
        # Drop the last "\n", or the header if no options or option groups:
        return "".join(result[:-1])
    def format_option_longhelp(self, formatter=None):
        if formatter is None:
            formatter = self.formatter
        formatter.store_option_strings(self)
        result = []
        result.append(formatter.format_heading(gettext("Options")))
        formatter.indent()
        if self.option_list:
            result.append(OptionContainer.format_option_help(self, formatter))
            result.append("\n")
        for group in self.option_groups:
            result.append(group.format_help(formatter))
            result.append("\n")
        formatter.dedent()
        # Drop the last "\n", or the header if no options or option groups:
        return "".join(result[:-1])
    def format_longhelp(self, formatter=None):
        if formatter is None:
            formatter = self.formatter
        result = []
        if self.usage:
            result.append(self.get_usage() + "\n")
        if self.description:
            result.append(self.format_description(formatter) + "\n")
        result.append(self.format_option_longhelp(formatter))
        result.append(self.format_epilog(formatter))
        return "".join(result)
    def print_help(self, filename=None):
        if filename is None:
            filename = self.help_output
        OptionParser.print_help(self, filename)
    def print_longhelp(self, filename=None):
        if filename is None:
            filename = self.help_output
        encoding = self._get_encoding(filename)
        filename.write(self.format_longhelp().encode(encoding, "replace"))
    def destroy(self):
        del self.command_list
        OptionParser.destroy(self)
