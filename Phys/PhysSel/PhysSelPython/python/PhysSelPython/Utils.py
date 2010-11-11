"""Set of utilities for handling and general manipulation of Configurables.

"""

__all__ = ('configurableExists',
           'isConfigurable')

__author__ = 'Juan Palacios palacios@physik.uzh.ch'


from GaudiConf.Configuration import allConfigurables
from GaudiKernel.Configurable import Configurable
from GaudiKernel.ConfigurableMeta import ConfigurableMeta

def configurableExists(name) :
    return allConfigurables.get(name) != None

def isConfigurable(alg) :
    return issubclass(type(alg), Configurable) or issubclass(type(alg), ConfigurableMeta)
