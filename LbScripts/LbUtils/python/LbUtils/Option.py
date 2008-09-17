from optparse import OptionParser, Option, OptionContainer
from LbUtils import Log, Env
import os, sys
import logging
from gettext import gettext


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


class Parser(OptionParser):
    def __init__(self, *args, **kwargs):
        kwargs["option_class"] = FallBackOption
        OptionParser.__init__(self, *args, **kwargs) #IGNORE:W0142
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
            if group.title != "Logging" and group.title != "Environment" :
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

    def print_longhelp(self, filename=None):
        if filename is None:
            filename = sys.stdout
        encoding = self._get_encoding(filename)
        filename.write(self.format_longhelp().encode(encoding, "replace"))
        