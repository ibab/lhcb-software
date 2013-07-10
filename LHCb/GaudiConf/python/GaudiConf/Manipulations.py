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


def fullNameConfigurables():
    """
    Simple function to find all defined configurables
    and make a dictionary of {fullName : conf}
    """
    import Gaudi.Configuration as GaudiConfigurables
    retdict={}
    for key in GaudiConfigurables.allConfigurables:
        retdict[GaudiConfigurables.allConfigurables[key].getFullName()]=GaudiConfigurables.allConfigurables[key]
    return retdict
    
def nameFromConfigurable(conf):
    """
    Safe function to get the full name of a configurable
    """
    if type(conf) is str:
        return conf
    elif hasattr(conf, "getFullName"):
        return conf.getFullName()
    return None

def configurableInstanceFromString(config):
    '''Get a configurable instance given only the string
    Uses  fullNameConfigurables() to get all defined configurables
    If this configurable has not been instantiated, I instantiate it'''
    
    import Gaudi.Configuration as GaudiConfigurables
    
    #if it's in Gaudi.Configuration with this name, return it
    
    if config in fullNameConfigurables():
        return fullNameConfigurables()[config]

    #might be using a short name instead:
    if config in GaudiConfigurables.allConfigurables:
        return GaudiConfigurables.allConfigurables[config]
    
    config=config.replace('::','__')
    
    #Try again with __ instead of :: to cope with namespaces!
    
    if config in fullNameConfigurables():
        return fullNameConfigurables()[config]
    
    if config in GaudiConfigurables.allConfigurables:
        return GaudiConfigurables.allConfigurables[config]
    
    #Since I didn't find it, I need to create it:
    wclass=None

    if hasattr(GaudiConfigurables, config.split('/')[0]):
        wclass = getattr(GaudiConfigurables,config.split('/')[0])
    else:
        import Configurables
        wclass = getattr(Configurables,config.split('/')[0])
        #otherwise it must be a configurable
        
    #check if it has an instance name
    if '/' not in config:
        return wclass()
    else:
        return wclass(name=config.split('/')[-1])

def removeConfigurables(conf_list):
    '''Helper: get rid of all configurables from a list'''
    from Gaudi.Configuration import allConfigurables
    conftodel=[k for k in allConfigurables if allConfigurables[k] in conf_list]
    for k in conftodel:
        del allConfigurables[k]

# Copied from HLT Conf, setting given configurables with a dictionary:

def forAllConf( head, prop_value_dict, types=[], force=False ) :
    """ Find all configurable algorithms and set certain properties
    
    head_conf: can be a sequence or a list, or a configurable to start with.
    prop_value_dict: A dictionary of Property: Value, e.g. {'OutputLevel':3}
    types: A list of types to check against, e.g. ['FilterDesktop','CombineParticles','DVAlgorithm'...], default empty list, doesn't check for types
    force: Overwrite properties even if they are already set, default False
    
    To obtain all configurables try something like:
    forAllConf(ApplicationMgr().TopAlg,{'OutputLevel':3})
    or:
    forAllConf(fullNameConfigurables(),keys(),{'OutputLevel':3})
    """
    if type(prop_value_dict) is not dict:
        raise TypeError("Hey, you need to give me a dictionary, you passed me a, "+str(type(prop_value_dict)))
    
    #recurse over lists
    if type(head) is list:
        for i in head: forAllConf(i,prop_value_dict,types,force)
    
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

def postConfForAll(head, prop_value_dict, types=[], force=False ) :
    """postConfigCallable with signature for forAllConf
    Append postConfigAction or forAllConf"""
    postConfigCallable(forAllConf,head,prop_value_dict, types=[], force=False)

