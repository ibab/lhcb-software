"""Module containing AutomaticDatas corresponding to common particles from CommonParticles.StandardBasic.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

__all__ = ['addParticleModule',
           'selections']

from sys import modules
_this = modules[__name__]
from CommonParticles import StandardBasic
from PhysSelPython.Wrappers import AutomaticData

selections = []

def addParticleModule(particleModule) :
    """
    Add all the common particles in particleModule so Selections module.
    """
    for loc in particleModule.locations.keys() :
        name = loc.split('/')[-1]
        global __all__
        __all__ += [name]
        global selections
        _loc = (loc+'/Particles').replace('/Particles/Particles',
                                          '/Particles')
        _loc = _loc.replace('//', '/')
        sel =  AutomaticData(Location =  loc)
        selections += [sel]
        setattr(_this,  name, sel)

addParticleModule(StandardBasic)

if __name__ == '__main__' :
    print '\nStandardParticles: available Selections:\n'
    for sel in selections :
        print '\t', sel.name()
