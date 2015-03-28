import collections
from itertools import ifilterfalse

def uniqueEverseen(iterable, key = None):
    "List unique elements, preserving order. Remember all elements ever seen."
    seen = set()
    seen_add = seen.add
    if key is None:
        for element in ifilterfalse(seen.__contains__, iterable):
            seen_add(element)
            yield element
    else:
        for element in iterable:
            k = key(element)
            if k not in seen:
                seen_add(k)
                yield element

def makeList(arg):
    if isinstance(arg, collections.Iterable) and not isinstance(arg, basestring):
        return arg
    else:
        return [arg]

def splitSpecs(specs, globalSpecs):
    hasTrackTagger = False
    try:
        from Configurables import TisTosTrackTagger
        hasTrackTagger = True
    except ImportError:
        pass

    if hasTrackTagger:
        globSpecs = {}
        localSpecs = {}
        for s in specs:
            if s in globalSpecs:
                globSpecs[s] = globalSpecs[s]
            else:
                localSpecs[s] = 0
    else:
        globSpecs = {}
        localSpecs = dict((s, 0) for s in specs)
        
    return localSpecs, globSpecs

def flatten(l):
    from Hlt2Stage import Hlt2Stage
    for i in l:
        if isinstance(i, Hlt2Stage):
            for j in flatten(i._deps()):
                yield j
            for j in flatten(i._inputs()):
                yield j
            if i: yield i
        elif i:
            yield i

def makeStages(stages, source, cuts):
    ## Return what we create directly and add what we depend on to stages.
    direct_stages = []
    for i in source:
        if hasattr(i, 'stage'):
            stgs = i.stage(stages, cuts)
            if type(stgs) in (list, tuple):
                direct_stages += list(stgs)
            else:
                direct_stages.append(stgs)
        else:
            direct_stages.append(i)
    return direct_stages
            
def allCuts(conf):
    from copy import deepcopy
    # Get a local instance of the cuts
    cuts = deepcopy(conf.getProps())
    common = cuts.get('Common', {})
    for k, v in cuts.iteritems():
        com = deepcopy(common)
        if k != 'Common':
            com.update(v)
            cuts[k] = com
    return cuts
