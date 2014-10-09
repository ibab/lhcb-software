"""Module containing AutomaticDatas corresponding to common particles from CommonParticles.StandardBasic.
"""

__author__ = 'Juan Palacios palacios@physik.uzh.ch'

__all__ = ['addParticleModule',
           'selections',
           'locations']

from sys import modules
_this = modules[__name__]
from CommonParticles import StandardBasic, StandardIntermediate
from PhysSelPython.Wrappers import AutomaticData

selections = []
locations  = {}

def addParticleModule(particleModule) :
    """
    Add all the common particles in particleModule so Selections module.
    """
    for loc in particleModule.locations.keys() :
        name = loc.split('/')[-2]
        global __all__
        __all__ += [name]
        global selections
        sel =  AutomaticData(Location =  loc)
        selections += [sel]
        setattr(_this,  name, sel)

def getStdPartAlgorithms():
    """
    Create the dictionary with the algorithms
    """
    global locations
    stdBasic = StandardBasic.locations
    locations.update(stdBasic)
    stdInter = StandardIntermediate.locations
    locations.update(stdInter)


addParticleModule(StandardBasic)
addParticleModule(StandardIntermediate)

getStdPartAlgorithms()

if __name__ == '__main__' :
    print '\nStandardParticles: available Selections:\n'
    for sel in selections :
        print '\t', sel.name()
