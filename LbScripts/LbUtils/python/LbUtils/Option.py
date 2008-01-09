from optparse import OptionParser, Option
from LbUtils import Log, Env
import os
import logging


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
             'fallback_conf']



class Parser(OptionParser):
    def __init__(self, *args, **kwargs):
        kwargs["option_class"] = FallBackOption
        OptionParser.__init__(self, *args, **kwargs)
        Log.addDefaultLogger(self)
        Env.addEnvironment(self)
    def add_option(self, *args, **kwargs):
        if kwargs.has_key("fallback_env") :
            kwargs["help"] += "\nThe fallback environment variable is set to %s" % kwargs["fallback_env"]
        if kwargs.has_key("fallback_conf") :
            kwargs["help"] += "\nThe fallback configuration is set to %s" % kwargs["fallback_conf"]
        OptionParser.add_option(self, *args, **kwargs)
    def check_values(self, values, args):
        log = logging.getLogger()
        for opt in self._get_all_options():
            fb_env = getattr(opt,"fallback_env")
            fb_conf = getattr(opt,"fallback_conf") 
            if fb_env or fb_conf :
                dest = getattr(opt, "dest")
                if not getattr(values, dest) :
                    if fb_env :
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
                        log.error("no value for %s" % dest)
        return (values, args)
