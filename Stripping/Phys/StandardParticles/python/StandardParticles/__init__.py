"""Module containing AutomaticDatas corresponding to common particles from CommonParticles.StandardBacis and CommonParticles.StandardIntermediate.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'


from sys import modules
_this = modules[__name__]
from CommonParticles import StandardBasic
from PhysSelPython.Wrappers import AutomaticData

def addModuleToSelections(particleModule) :
    """
    Add all the common particles in particleModule so Selections module.
    """
    for loc in particleModule.locations.keys() :
        name = loc.split('/')[-1]
        setattr(_this,  name, AutomaticData(Location =  loc))

addModuleToSelections(StandardBasic)
