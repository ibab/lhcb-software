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
