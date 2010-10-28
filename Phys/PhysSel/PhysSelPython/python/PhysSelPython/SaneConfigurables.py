"""Module containing fixed versions of insane configurables.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'


from ConfigurableHospital import memoized_sanitise
from CrazyConfigurables import configurables

def addConfigurables(configurables) :
    sanitise(configurables)

from sys import modules
this = modules[__name__]
import Configurables as _crazyConfigurables
sanitise = memoized_sanitise(_crazyConfigurables, this)
sanitise( configurables() )

def cureConfigurables(configurables) :
    """
    Take configurables, cure them, and add them to this module.
    
    """
    sanitise(configurables)
