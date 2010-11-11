"""Module with all kinds of tools to cure the sick, sick Configurables.

"""
from GaudiConf.Configuration import allConfigurables
from GaudiKernel.Configurable import Configurable
from GaudiKernel.ConfigurableMeta import ConfigurableMeta
from PhysSelPython.Utils import configurableExists, isConfigurable
import inspect

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

__all__ = ('filterConfigurables',
           'getConfigurablesFromModule',
           'SaneConfigurable',
           'sanitise')



def isIterable(obj) :
    '''
    Test if an object is iterable but not a string type or Configurable.
    '''
    return hasattr(obj, "__iter__") and not isinstance(obj, basestring) and not isConfigurable(obj)


def filterConfigurables(listOfSymbols) :
    """
    Return a list containing all the Configurables present in an arbitrery list of symbols.
    """
    return filter(isConfigurable, listOfSymbols)

def getConfigurablesFromModule(confModule) :
    """
    Return a list with all the configurables in module confModule
    usage:
    import SomeModule.SomeSubModule
    configurables = getConfigurablesFromModule(SomeModule.SomeSubModule)
    """
    # remove all special names
    algNames = filter(lambda x : x[0]!='_',
                      confModule.__dict__.keys())

    # load the symbols
    algs = [getattr( confModule, x) for x in algNames]

    # Remove anything that doesn't belong to this module (i.e. imports)
    algs = filter(lambda x : x.__module__ == confModule.__name__,
                  filterConfigurables(algs))
    
    return algs

_originalCloner = Configurable.clone

def _saneClone(obj, name, **kwargs) :
    print 'Try to clone to a new configurable', name
    if configurableExists(name) :
        raise ValueError('Configurable '+ name + ' already exists. Pick a new name')
    else :
        print 'Configurable', name, 'does not exist. Proceed with cloning.'
        return _originalCloner(obj,  name, **kwargs)

class SaneConfigurable(object) :

    def __init__(self, configurableType) :
        if not isConfigurable(configurableType) :
            raise TypeError('SaneConfigurable only works for Configurables!')

        configurableType.clone = _saneClone
        self.configurableType = configurableType
        self.name = type(configurableType).__name__
        self.__name__ = self.name
        
    def __call__(self, name='', **kwargs) :
        if name == '' :
            name = self.name
        print 'Try to instantiate a new configurable', name
        if configurableExists(name) :
            raise ValueError('Configurable '+ name + ' already exists. Pick a new name')
        else :
            print 'Configurable', name, 'does not exist. Build it.'
            saneConfig = self.configurableType(name, **kwargs)
            return saneConfig

import Configurables as _crazyConfigurables

class memoized_sanitise(object) :
    """
    Take a list consisting of Configurables, Configurable names, or Configurable modules,
    and replace each Configurable by their sane counterpart in the Configurables module.
    List can contain any mix of Configurable types, their string counterparts, of modules
    containing Configurables.
    """
    _cache = []
    def __init__(self, sourceModule, targetModule) :
        self._source = sourceModule
        self._target = targetModule

    def __call__(self, algs) :

        if not isIterable(algs) :
            if inspect.ismodule(algs) :
                print 'Dealing with a module'
                algs = getConfigurablesFromModule(algs)
            else :
                'dealing with a single instance'
                algs = [algs]

        self.processAlgs(algs)
        
    def processAlgs(self, algs) :
        for alg in algs :
            print 'Bringing sanity to', alg
            if inspect.ismodule(alg) :
                'Deal with module'
                self.processAlgs(getConfigurablesFromModule(alg))
            else :
                'not module', alg
                if alg in memoized_sanitise._cache or issubclass(type(alg), SaneConfigurable) :
                    pass
                else :
                    memoized_sanitise._cache += [alg]
                    crazyAlg = alg
                    if type(alg) == str :
                        crazyAlg = self._source.__getattr__(alg)
                    elif isConfigurable(alg) :
                        alg = alg.__name__
                    saneAlg =  SaneConfigurable(crazyAlg)
                    self._target.__setattr__(alg, saneAlg)

sanitise = memoized_sanitise(_crazyConfigurables, _crazyConfigurables)
