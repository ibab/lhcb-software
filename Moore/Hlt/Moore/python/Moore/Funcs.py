
import GaudiKernel.ProcessJobOptions
from  ctypes import c_uint


## HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

# canonicalize tck  -- eats integer + string, returns canonical string
def _tck(x) :
    if type(x) == str and x[0:2] == '0x' : return '0x%08x'%int(x,16)
    return '0x%08x'%int(x)
    
from Gaudi.Configuration import appendPostConfigAction

def fixDQ():
    from Configurables import CondDBDispatcherSvc
    c = CondDBDispatcherSvc("MainCondDBReader")
    del c.Alternatives['/Conditions/DQ']
#appendPostConfigAction(fixDQ)

def _choose(orig, repl, m):
    if m!=orig:
        return m
    return repl

#Ternary operator not valid for old python versions, replacing...
_replace = lambda orig, repl, members : [ _choose(orig,repl,m) for m in members ]
_remove  = lambda remove,     members : [ m for m in members if m.name() not in remove ]
_remove_re  = lambda re,      members : [ m for m in members if not re.match(m.name()) ]



## FIXME: how to catch HltUnit's RunAll???
def _walkAlgorithms( c , descend = [ 'Members','Prescale','ODIN','L0DU','HLT','Filter0','Filter1','Postscale'] ) :
    for p in descend :
        if not hasattr(c,p) : continue
        x = getattr(c,p)
        import GaudiKernel
        if isinstance(x,GaudiKernel.Configurable.Configurable) : x = [x]
        for i in x :
            for j in _walkAlgorithms(i,descend) : yield j
    yield c

def _updateProperties( top, table, value ) :
    for conf in _walkAlgorithms( top ) :
        prop = table.get( conf.getType().replace(':','_') , None )
        if prop : setattr( conf, prop, value )

def _zipdict(dict1,dict2):
    """
    return a dictionary with all entries of both dict1 and dict2
    if entry is a dictionary, recurse, if entry is a list, append
    """
    retdict={}
    
    for k,v in dict1.iteritems():
        #unique to dict 1
        if k not in dict2:
            retdict[k]=v
            #else they are not unique, I need to decide what to do about that...
        elif type(v) != type(dict2[k]):
            raise TypeError("Cannot zip dicts with identical entries who differ in type! "+str(type(v))+"!="+str(type(dict2[k]))+" for "+str(k))
        elif type(v) is dict: 
            #recurse, transforms are dicts of dicts of dicts...
            retdict[k]=_zipdict(v,dict2[k])
        elif type(v) is list:
            retdict[k]=v+[a for a in dict2[k] if a not in v]
        else:
            raise TypeError("No rule to zip entries of type "+str(type(v))+" for "+str(k))
    
    for k,v in dict2.iteritems():
        #unique to dict 2
        if k not in retdict:
            retdict[k]=v
    
    return retdict

def _mergeTransform(trans):
    """
    Concatenate transforms into the HltConfigSvc
    """
    from Configurables import HltConfigSvc
    prop="ApplyTransformation"
    svc=HltConfigSvc()
    if (svc.getProp(prop) is not None and type(svc.getProp(prop)) is dict and len(svc.getProp(prop))):
        #if it's already set, or the default is some non-zero dictionary, merge it
        svc.setProp(prop,_zipdict(svc.getProp(prop),trans))
    else:
        #else set it
        svc.setProp(prop,trans)
