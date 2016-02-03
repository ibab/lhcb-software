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
from GaudiKernel.Configurable import Configurable, ConfigurableGeneric

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
    # Configurables typenames use '__' as namespace separator
    typename = typename.replace('::', '__')

    try:
        return (GaudiConfigurables.allConfigurables.get(name) or
                getattr(Configurables, typename)(name))
    except TypeError:
        # this can happen during build
        if Configurables.ignoreMissingConfigurables:
            return None
        raise

def removeConfigurables(conf_list):
    '''Helper: get rid of all configurables from a list'''
    from Gaudi.Configuration import allConfigurables
    conftodel=[k for k in allConfigurables if allConfigurables[k] in conf_list]
    for k in conftodel:
        del allConfigurables[k]


def recurseConfigurables(func, head=None, descend_properties=[], descend_tools=False):
    """Recursively visit configurables and call a function for each.
    
    Note that the function is called before recursing over properties
    or tools, so if it modifies them (e.g. adds tools), this will be
    reflected in the recursion.
    The behaviour of the removed forAllConf / postConfForAll can be
    achieved with:
    
    func = partial(setPropertiesAndAddTools, properties=..., tools_per_type=..., force=...)
    descend_properties = ['Members', 'Filter0', 'Filter1', 'TopAlg']
    appendPostConfigAction(partial(recurseConfigurables, func, descend_properties=descend_properties, descend_tools=...))
    
    Args:
        func: A function taking a single argument to be called for each
            visited configurable.
        head: A configurable or a list of configurables from which to
            start recursing. If None is given (default), all instantiated
            configurables plus ApplicationMgr().TopAlg will be used.
        descend_properties: List of properties to recurse over.
        descend_tools: Whether to recurse over tools or not.
    
    Returns:
        A set of all configurables that were visited.
    """
    
    if head is None:
        from Gaudi.Configuration import ApplicationMgr
        head = [ApplicationMgr()] + ApplicationMgr().TopAlg
        head += GaudiConfigurables.allConfigurables.values()

    visited = set()
    __recurseConfigurables(func, head, descend_properties, descend_tools, visited)
    return visited

def __recurseConfigurables(func, head, properties, tools, visited):
    if isinstance(head, list):
        for i in head:
            __recurseConfigurables(func, i, properties, tools, visited)
        return

    if isinstance(head, str):
        try:
            head = configurableInstanceFromString(head)
        except (AttributeError, TypeError):
            return  # I cannot find the configurable, skip it
        
    if isinstance(head, Configurable) and head not in visited:
        visited.add(head)
        func(head)
        if tools:
            for tool in head.getTools():
                __recurseConfigurables(func, tool, properties, tools, visited)
        if properties is not None:
            prop_values = [getattr(head, p, None) for p in properties]
        else:
            prop_values = head.getProperties().values()
        for value in prop_values:
            if isinstance(value, (list, str, Configurable)):
                __recurseConfigurables(func, value, properties, tools, visited)

def setPropertiesAndAddTools(configurable, properties, tools_per_type={}, force=False):
    """Set properties of, and add private tools to a configurable.
    
    Note that properties of ConfigurableGeneric instances are only set
    if they have been previously set and force=True. (The reason is that
    ConfigurableGeneric-s do not know what properties are valid).
    
    Args:
        configurable: The configurable to modify.
        properties: A dictionary of property names and values. If a
            property does not exist for the given configurable, it is
            ignored.
        tools_per_type: A dictionary of the form {"configurable type":
            ["tooltype/toolname", ...]}. For example:
            {"CaloSinglePhotonAlg": ["CaloECorrection/ECorrection"]}
    """
    is_generic = isinstance(configurable, ConfigurableGeneric)
    for name, value in properties.iteritems():
        if force or (not configurable.isPropertySet(name)):
            if is_generic and not hasattr(configurable, name):
                continue  # skip unset properties of ConfigurableGeneric-s
            try:
                configurable.setProp(name, value)
            except AttributeError:
                pass
    tools = tools_per_type.get(configurable.getType(), [])
    for spec in tools:
        tool = addPrivateToolFromString(configurable, spec)
