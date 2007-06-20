# module with functions that deals LHCb MC particles and vertices
# author: D. Martinez, J.A. Hernando

from ParticleTranslator import PTranslate
from math import *

import gaudimodule
Track = gaudimodule.gbl.LHCb.Track
State = gaudimodule.gbl.LHCb.State

DEBUG = False

def decayName(pars):
    """ providing a list of particles [pars], return a string with the decay
    the first particle should be the mother
    arguments [pars] (where pars[0] = B0), i.e. returns 'B0->K-pi+'
    """
    pid = pars[0].particleID().pid()
    decay = PTranslate(pid)+ "->"
    for par in pars[1:]:
        pid = par.particleID().pid()
        decay += PTranslate(pid)
    if (DEBUG): print " decay: ", decay
    return decay
    
def makeTrack(mcpar):
    """ make a Track object from a MCParticle
    """
    track = Track()
    state = State()
    over = mcpar.originVertex()
    x = over.position().x()
    y = over.position().y()
    z = over.position().z()
    px = mcpar.momentum().x()
    py = mcpar.momentum().y()
    pz = mcpar.momentum().z()
    pp = sqrt(px*px+py*py+pz*pz)
    pid = mcpar.particleID().pid()
    q = pid/abs(pid)
    state.setState(x,y,z,px/pz,py/pz,q/pp)
    track.addToStates(state)
    if (DEBUG): print " makeTrack: ",track
    return track

def roots(mcpars,pid=531):
    """ Returns a list with the initial particles with pid (i.e Bs's)
    """
    broot = []
    for i in range(mcpars.size()):
        mcpar=mcpars.containedObjects()[i]
        if ((mcpar.particleID().abspid() == pid) and
            (mcpar.mother() == None)):
            broot.append(mcpar)
    if (DEBUG): print " roots: ",broot
    return broot

def rootMother(mcpar,i=0):
    """Returns the mother of the MC particle mcpar
    and the number of generations to the mother
    """
    mom = mcpar.mother()
    if (mom == None): return mcpar,i
    i += 1
    return rootMother(mom,i)
    
def findDecay(mcpars,ngenerations = 1, pid=531):
    """ returns a list of lists, each one containing the search decay
    the mother of the decay has pid (i.e 531 Bs)
    the number of generation to search for can be indicated (default 1)
    """
    mroots = roots(mcpars,pid)
    bsignal = []
    for iroot in mroots:
        if (not abs(iroot.particleID().pid()) == pid): continue
        bsons = []
        for mcpar in mcpars:
            i = 0
            (mom,i) = rootMother(mcpar,i)
            if ((mom.key() == iroot.key()) and (i <= ngenerations)):
                bsons.append(mcpar)
        bsignal.append(bsons)
    if (DEBUG): print " findDecay: size ",len(bsignal)    
    return bsignal


