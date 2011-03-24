import os, sys
import logging




class Environment:
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

        # the keys of the environment dictionary are case insensitive on windows
        if sys.platform.startswith("win"):
            self._fixKey = lambda key: key.upper()
        else:
            self._fixKey = lambda key: key

        
    def __setitem__(self,key,value):
        """
        Set an environment variable recording the previous value.
        """
        key = self._fixKey(key)
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
        key = self._fixKey(key)
        return self.env[key]
    
    def __delitem__(self,key):
        """
        Unset an environment variable.
        Needed to provide the same interface as os.environ.
        """
        key = self._fixKey(key)
        if key not in self.env :
            raise KeyError(key)
        self.old_values[key] = self.env[key]
        del self.env[key]
        log = logging.getLogger()
        log.info("removed %s from environment" % key)

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
        key = self._fixKey(key)
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
        key = self._fixKey(key)
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
        key = self._fixKey(key)
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


class Aliases(Environment) :
    def __init__(self, keep_same=False):
        Environment.__init__(self, orig=dict(), keep_same=keep_same)
        self._fixKey = lambda key: key
    def gen_script(self,shell_type):
        """
        Generate a shell script to reproduce the changes in the aliases.
        """
        shells = [ 'csh', 'sh', 'bat' ]
        if shell_type not in shells:
            raise RuntimeError("Shell type '%s' unknown. Available: %s" % (shell_type, shells))
        out = ""
        for key in self.old_values:
            if key not in self.env:
                # unset variable
                if shell_type == 'csh':
                    out += 'unalias %s\n' % key
                elif shell_type == 'sh':
                    out += 'unalias %s\n' % key
            else:
                # set variable
                if shell_type == 'csh':
                    out += 'alias %s "%s"\n' % (key, self.env[key])
                elif shell_type == 'sh':
                    out += 'alias %s="%s"\n' % (key, self.env[key])
        return out

_globalenv = Environment()

def getDefaultEnv():
    """ return global instance of the environment """
    return _globalenv

class EnvVarException(Exception):
    pass

def varArgSplit(vararg):
    vardict = dict()
    varlist = []
    pathsep = [ x for x in ";" if vararg.find(x) != -1 ]
    if len(pathsep) > 0:
        if len(pathsep) == 1 :
            varlist.append(vararg.split(pathsep))
        else :
            raise EnvVarException, "Mixing of path separator symbol" 
    else:
        varlist.append(vararg)
    for c in varlist:
        varsep = [ x for x in "=" if c.find(x)!= -1 ]
        if len(varsep) > 0 :
            if len(varsep) == 1 :
                varcontent = c.split(varsep)
                value = varcontent.pop()
                for v in varcontent:
                    vardict[v] = value
            else :
                raise EnvVarException, "Mixing of var separator symbol"
        else :
            vardict[c] = None
    return vardict

def setVarCallBack(option, opt_str, value, parser): #IGNORE:W0613
    log = logging.getLogger()
    env = getDefaultEnv()
    edict = varArgSplit(value)
    if opt_str == "--env" :
        for e in edict.keys() :
            if edict[e] :
                env[e] = edict[e]
                log.info("setting %s to %s" % (e,env[e]))
            else :
                env[e] = ""
                log.info("setting %s" % e )
    if opt_str == "--unenv" :
        for e in edict.keys() :
            if not edict[e] :
                del env[e]
                log.info("removing %s" % e)
            else :
                env[e] = env[e].replace(edict[e],"")
                log.info("changed %s to %s" % (e, env[e]))

def setPathCallBack(option, opt_str, value, parser): #IGNORE:W0613
    log = logging.getLogger()
    env = getDefaultEnv()
    edict = varArgSplit(value)
    if opt_str == "--path-append" :
        for e in edict.keys() :
            if edict[e] :
                env[e] = os.pathsep.join(env[e].split(os.pathsep)+ [edict[e]])
                log.info("appending %s to %s" % (edict[e], e) )
                log.debug("%s is %s" % (e, env[e]) )
    if opt_str == "--path-prepend" :
        for e in edict.keys() :
            if edict[e] :
                env[e] = os.pathsep.join([edict[e]]+env[e].split(os.pathsep))
                log.info("prepending %s to %s" % (edict[e], e) )
                log.debug("%s is %s" % (e, env[e]) )
    if opt_str == "--path-remove" :
        for e in edict.keys() :
            if edict[e] :
                envlist = env[e].split(os.pathsep)
                for l in envlist :
                    if l.find(edict[e]) != -1 :
                        envlist.remove(l)
                        log.info("removing %s from %s" % (l, e) )
                env[e] = os.pathsep.join(envlist)
                log.debug("%s is %s" % (e, env[e]) )

def addEnvironment(parser):
    grp = parser.add_option_group("Environment")
    parser.set_defaults(verbose=False)
    grp.add_option("--env",
                      action = "callback",
                      callback = setVarCallBack,
                      type = "string",
                      nargs = 1,
                      metavar = "VAR[=value]",
                      help = "Add or override env variable of the environment.\n"
                      "--env VAR:value (set the variable VAR to value)\n"
                      "--env VAR (set the variable VAR to empty)")
    grp.add_option("--unenv",
                      action = "callback",
                      callback = setVarCallBack,
                      type = "string",
                      nargs = 1, 
                      metavar = "VAR[=value]",
                      help = "remove env variable or part of it. \n"
                      "--unenv VAR (remove the variable VAR)\n"
                      "--unenv VAR:value (remove every occurence \n"
                      "of value from the variable VAR)")
    grp.add_option("--path-append",
                      action = "callback",
                      callback = setPathCallBack,
                      type = "string",
                      nargs = 1, 
                      metavar = "VAR=value",
                      help = "append a component to a path variable\n"
                      "--path-append VAR:value")
    grp.add_option("--path-prepend",
                      action = "callback",
                      callback = setPathCallBack,
                      type = "string",
                      nargs = 1, 
                      metavar = "VAR=value",
                      help = "prepend a component to a path variable\n"
                      "--path-prepend VAR:value")
    grp.add_option("--path-remove",
                      action = "callback",
                      callback = setPathCallBack,
                      type = "string",
                      nargs = 1, 
                      metavar = "VAR=str",
                      help = "remove a component to a path variable\n"
                      "--path-remove VAR:regexp")
    return _globalenv
