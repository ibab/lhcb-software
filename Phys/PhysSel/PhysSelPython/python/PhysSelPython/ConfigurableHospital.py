"""Module with all kinds of tools to cure the sick, sick Configurables.

"""
from GaudiKernel.Configurable import Configurable
from GaudiConfUtils import isIterable
from GaudiConfUtils import isConfigurable
from GaudiConfUtils import getConfigurablesFromModule
from GaudiConfUtils import configurableExists
import inspect

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

__all__ = ('SaneConfigurable',
           'sanitise')

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
