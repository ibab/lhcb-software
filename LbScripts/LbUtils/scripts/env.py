#!/usr/bin/env python
"""
Small script to execute a command in a modified environment (see man 1 env).
"""
import os

def set_env(env, set = [], unset = [], append = [], prepend = []): #@ReservedAssignment
    """
    Manipulate the dictionary-like object 'env' according to the prescriptions in
    the lists 'unset', 'set', 'append' and 'prepend' (in this order).
    The lists must contain strings of the format 'NAME=VALUE' except for 'unset'
    which requires only 'NAME'.
    """
    def parse(x):
        """split the "NAME=VALUE" string into the tuple ("NAME", "VALUE")
        replacing '[:]' with os.pathsep in VALUE"""
        n, v = x.split('=', 1)
        return n, v.replace('[:]', os.pathsep)
    def dictlist(l):
        """create a dictionary from the list of pairs by appending to lists"""
        d = {}
        for n, v in l:
            if n in d:
                d[n].append(v)
            else:
                d[n] = [v]
        return d
    # remove the 'unset' variables
    for n in unset:
        if n in env:
            del env[n]
    # set the requested variables
    env.update(map(parse, set))
    # append
    for n, v in dictlist(map(parse, append)).items():
        if n in env:
            v.insert(0, env[n]) # add old value at the beginning
        env[n] = os.pathsep.join(v)
    # prepend
    for n, v in dictlist(map(parse, reversed(prepend))).items():
        if n in env:
            v.append(env[n]) # add old value at end
        env[n] = os.pathsep.join(v)

    return env

def parse_args():
    from optparse import OptionParser
    parser = OptionParser(prog = "env",
                          usage = "Usage: %prog [OPTION]... [NAME=VALUE]... [COMMAND [ARG]...]",
                          description = "Set each NAME to VALUE in the environment and run COMMAND.",
                          epilog = "The operations are performed in the order: unset, set, append, "
                                   "prepend. If no COMMAND, print the resulting environment." )
    parser.add_option("-i", "--ignore-environment",
                      action = "store_true",
                      help = "start with an empty environment")
    parser.add_option("-u", "--unset",
                      metavar = "NAME",
                      action = "append",
                      help = "remove variable from the environment")
    parser.add_option("-s", "--set",
                      metavar = "NAME=VALUE",
                      action = "append",
                      help = "set the variable NAME to VALUE")
    parser.add_option("-a", "--append",
                      metavar = "NAME=VALUE",
                      action = "append",
                      help = "append VALUE to the variable NAME (with a '%s' as separator)" % os.pathsep)
    parser.add_option("-p", "--prepend",
                      metavar = "NAME=VALUE",
                      action = "append",
                      help = "prepend VALUE to the variable NAME (with a '%s' as separator)" % os.pathsep)
    parser.add_option("-x", "--xml",
                      action = "append",
                      help = "XML file describing the changes to the environment")
    parser.add_option("--sh",
                      action = "store_const", const = "sh", dest = "shell",
                      help = "Print the environment as shell commands for 'sh'-derived shells.")
    parser.add_option("--csh",
                      action = "store_const", const = "csh", dest = "shell",
                      help = "Print the environment as shell commands for 'csh'-derived shells.")
    parser.add_option("--py",
                      action = "store_const", const = "py", dest = "shell",
                      help = "Print the environment as Python dictionary.")
    parser.disable_interspersed_args()
    parser.set_defaults(unset = [],
                        set = [],
                        append = [],
                        prepend = [],
                        xml = [],
                        ignore_environment = False)

    return parser.parse_args()

def envFromXML(env, xmlfiles):
    '''
    Extend the environment declared in the dictionary env with the actions
    in the XML files specified in the list 'xmlfiles'.
    '''
    from EnvConfig import Control
    control = Control.Environment()
    # declare scalar and list variables ("*PATH*" and "*DIRS*" are lists)
    for k, v in env.items():
        if 'PATH' in k or 'DIRS' in k:
            t = 'list'
        else:
            t = 'scalar'
        control.declare(k, t, False)
        control.set(k, v)

    for f in xmlfiles:
        control.loadXML(f)

    return control.vars()

def main():
    opts, args = parse_args()

    # find the 'set' arguments in the list of arguments
    i = 0
    argc = len(args)
    while (i < argc) and ("=" in args[i]):
        i += 1
    opts.set.extend(args[:i])
    cmd = args[i:]

    if opts.shell and cmd:
        print >> sys.stderr, "Invalid arguments: --%s cannot be used with a command" % opts.shell
        return 2

    # prepare initial dictionary
    if opts.ignore_environment:
        env = {}
    else:
        env = dict(os.environ)

    if opts.xml:
        env = envFromXML(env, opts.xml)

    env = set_env(env,
                  set = opts.set, unset = opts.unset,
                  append = opts.append, prepend = opts.prepend)

    if "LD_LIBRARY_PATH" in env:
        # replace LD_LIBRARY_PATH with the corresponding one on other systems
        if sys.platform.startswith("win"):
            other = "PATH"
        elif sys.platform.startswith("darwin"):
            other = "DYLD_LIBRARY_PATH"
        else:
            other = None
        if other:
            if other in env:
                env[other] = env[other] + os.pathsep + env["LD_LIBRARY_PATH"]
            else:
                env[other] = env["LD_LIBRARY_PATH"]
            del env["LD_LIBRARY_PATH"]

    if not cmd:
        if opts.shell == 'py':
            from pprint import pprint
            pprint(env)
        else:
            format = {'sh':  "export %s='%s'",
                      'csh': "setenv %s '%s'"}.get(opts.shell, "%s=%s")
            for nv in sorted(env.items()):
                print format % nv
        return 0
    else:
        from subprocess import Popen
        return Popen(cmd, env=env).wait()

if __name__ == "__main__":
    import sys
    sys.exit(main())
