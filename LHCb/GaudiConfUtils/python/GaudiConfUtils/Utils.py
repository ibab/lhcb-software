"""Module with all kinds of tools to manipulate Configurables and Configurable modules.

@author Juan Palacios palacios@physik.uzh.ch

"""

from GaudiKernel.Configurable import Configurable
from GaudiKernel.ConfigurableMeta import ConfigurableMeta
import inspect

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

__all__ = ('isIterable',
           'isConfigurable',
           'configurableExists',
           'filterConfigurables',
           'getConfigurablesFromModule')

def configurableExists(name) :
    """
    Return True if a configurable exists in the Configurables module.
    """
    return Configurable.allConfigurables.get(name) != None

def isConfigurable(alg) :
    """
    Return True if an object is a Configurable.
    """
    return issubclass(type(alg), Configurable) or issubclass(type(alg), ConfigurableMeta)

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
