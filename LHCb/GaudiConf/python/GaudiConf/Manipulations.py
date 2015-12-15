# -*- coding: utf-8 -*-
"""
Simple python functions for setting common options to all configurables.
Uses elements from HLTConf and GaudiConf

Usage, something like:

forAllConf(ApplicationMgr().TopAlg,{'OutputLevel':3})
or
forAllConf(fullNameConfigurables().keys(),{'OutputLevel':3})

or same with postConfForAll...

"""
# Copied from GaudiConf, IOHelper. Manipulators of configurables.
import Gaudi.Configuration as GaudiConfigurables
import Configurables

def fullNameConfigurables():
    """
    Simple function to find all defined configurables
    and make a dictionary of {fullName : conf}
    """
    return dict((c.getFullName(), c)
                for c in GaudiConfigurables.allConfigurables.itervalues())

def nameFromConfigurable(conf):
    """
    Safe function to get the full name of a configurable
    """
    if type(conf) is str:
        return conf
    elif hasattr(conf, "getFullName"):
        return conf.getFullName()
    return None

def configurableClassFromString(config):
    '''Get a configurable class given only the string'''
    config = config.split('/')[0]
    config = config.replace('::', '__')

    try:
        return getattr(Configurables, config)
    except AttributeError:
        return None

def addPrivateToolFromString(amother,atool):
    '''
    Add a private tool correctly and return it, given a tool as "Type/InstanceName"
    If it already exists and is of the correct type, great.
    '''
    if type(amother) is str:
        amother=configurableInstanceFromString(amother)
    toolClass=configurableClassFromString(atool)
    toolName=atool.split("/")[-1]
    if hasattr(amother,toolName):
        if getattr(amother,toolName).getType()==toolClass.getType():
            return getattr(amother,toolName)
        raise TypeError("This configurable already has a private tool with that name, but of a different type! " + amother.getFullName() + " Expected: " + toolClass.getType() + " got " + getattr(amother,toolName).getType())
    amother.addTool(toolClass,name=toolName)
    return getattr(amother,toolName)

def configurableInstanceFromString(config):
    '''Get a configurable instance given a type/name the string.

    If the configurable was not instantiated yet, instantiate it.'''

    if '/' in config:
        typename, name = config.split('/', 1)
    else:
        typename = name = config

    try:
        return (GaudiConfigurables.allConfigurables.get(config) or
                getattr(Configurables, typename)(name))
    except (AttributeError, TypeError):
        # this is if the type is not in Configurables,
        # so we try replacing the namespace separator
        # FIXME: the logic is wrong because we replace '::' not only in the typename,
        #        but it's the same logic that was there before the change
        return configurableInstanceFromString(config.replace('::', '__'))

def removeConfigurables(conf_list):
    '''Helper: get rid of all configurables from a list'''
    from Gaudi.Configuration import allConfigurables
    conftodel=[k for k in allConfigurables if allConfigurables[k] in conf_list]
    for k in conftodel:
        del allConfigurables[k]


# Copied from HLT Conf, setting given configurables with a dictionary:

def forAllConf( head=None, prop_value_dict={}, types=[], force=False, tool_value_dict={} , recurseToTools=False) :
    """ Find all configurable algorithms and set certain properties

    head: can be a sequence or a list, or a configurable to start with.
    if None is given, allConfigurables plus ApplicationMgr().TopAlg will be used
    prop_value_dict: A dictionary of Property: Value, e.g. {'OutputLevel':3}
    types: A list of types to check against, e.g. ['FilterDesktop','CombineParticles','DVAlgorithm'...], default empty list, doesn't check for types
    force: Overwrite properties even if they are already set, default False
    tool_value_dict dictionary of dictionaries, first add tools then set their properties.

    To obtain all configurables try something like:
    forAllConf(ApplicationMgr().TopAlg,{'OutputLevel':3})
    or:
    forAllConf(fullNameConfigurables().keys(),{'OutputLevel':3})
    or:
    forAllConf(head=None, prop_value_dict={'OutputLevel':3})
    (last if you want to find all configurable which exist during a
    postconfig action, for example)
    """
    if type(prop_value_dict) is not dict:
        raise TypeError("Hey, you need to give me a dictionary, you passed me a, "+str(type(prop_value_dict)))
    if type(tool_value_dict) is not dict:
        raise TypeError("Hey, you need to give me a dictionary, you passed me a, "+str(type(prop_value_dict)))

    if head is None:
        from Gaudi.Configuration import ApplicationMgr
        head=[ApplicationMgr()]+fullNameConfigurables().values()

    #recurse over lists
    if type(head) is list:
        for i in head: forAllConf(i,prop_value_dict,types,force, tool_value_dict)
        return
    #interpret strings:
    if type(head) is str:
        try:
            head=configurableInstanceFromString(head)
        except:
            #I cannot find the configurable, skip it
            return


    if head is None:
        return
    #print "attempting",  nameFromConfigurable(head)
    if (not len(types)) or head.getType() in types:
        #recurse over tools
        if len(tool_value_dict):
            for k,v in tool_value_dict.iteritems():
                tool=addPrivateToolFromString(head,k)
                forAllConf(tool,prop_value_dict=v,types=[],force=force,recurseToTools=recurseToTools)
        if recurseToTools:
            forAllConf(head.getTools(),prop_value_dict,types=[],force=force,recurseToTools=recurseToTools)
        for prop in prop_value_dict:
            if hasattr(head,prop) or (hasattr(head,"properties") and prop in head.properties()) or(hasattr(head,"__slots__") and prop in head.__slots__):
                if force or (not head.isPropertySet(prop)):
                    head.setProp(prop,prop_value_dict[prop])
                    #print "set", nameFromConfigurable(head)

    #iterate/recurse over all subparts
    for p in [ 'Members','Filter0','Filter1','TopAlg' ] :
        if not hasattr(head,p) : continue
        seq = getattr(head,p)
        forAllConf(seq,prop_value_dict,types,force)

def postConfigCallable(*args,**kwargs):
    """
    Setup a post comfig action for any function requiring arguments

    usage:

    postConfigCallable(<afunction>,<arg1>,...,<kw1>=<kwarg1>,...)

    first unnamed argument MUST be a function to call in postConfig

    all other named or unnamed arguments are passed into the function

    """
    from Gaudi.Configuration import appendPostConfigAction
    import collections

    class dummyPostConf(object):
        """dummy class to wrap function call"""
        def __init__(self,thefunction,theargs,thekwargs):
            self.thefunction=thefunction
            self.theargs=theargs
            self.thekwargs=thekwargs
        def method(self):
            return self.thefunction(*self.theargs,**self.thekwargs)

    if not len(args):
        raise ValueError("You must pass at least one nameless arg, the function to call!")
    if not isinstance(args[0], collections.Callable):
        raise TypeError("The first nameless argument must be a callable function!")
    mydummy=None
    if len(args)==1:
        mydummy=dummyPostConf(args[0],kwargs)
    else:
        mydummy=dummyPostConf(args[0],args[1:],kwargs)
    appendPostConfigAction(mydummy.method)

def postConfForAll(head=None, prop_value_dict={}, types=[], force=False, tool_value_dict={} , recurseToTools=False) :
    """postConfigCallable with signature for forAllConf
    Append postConfigAction or forAllConf"""
    postConfigCallable(forAllConf,head,prop_value_dict, types=types, force=force, tool_value_dict=tool_value_dict, recurseToTools=recurseToTools)

