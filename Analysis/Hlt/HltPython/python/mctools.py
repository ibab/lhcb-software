#  Module with functions that deal with MC particles and vertices
#  @date 2007-11-28
#  @author Diego Martinez diego.martinez.santos@cern.ch
#  @author Jose Angel Hernando jose.angel.hernando-morata@cern.ch
#  @author Hugo RUIZ hugo.ruiz@cern.ch
# =============================================================================
""" Module with functions that deal with MC particles and vertices """
# =============================================================================
__author__ = "Diego MARTINEZ diego.martinez.santos@cern.ch"
__author__ = "Jose Angel HERNANDO jose.angel.hernando-morata@cern.ch"
__author__ = "Hugo RUIZ hugo.ruiz@cern.ch"
# =============================================================================


from ParticleTranslator import PTranslate
from math import *

import gaudimodule
Track = gaudimodule.gbl.LHCb.Track
State = gaudimodule.gbl.LHCb.State

DEBUG = False

class IDLinker:

    def __init__(self,TES):
        """ create a user friendly kinker table ID->MCParticle
        requires that in TES the link at 'Link/Pat/LHCbID' and mcparticles at 'MC/Particles'
        """
        self._link = TES["Link/Pat/LHCbID"]
        self._ref = gaudimodule.gbl.LHCb.LinkReference()
        self._mcpars = TES["MC/Particles"]
        self._Null = None
        if ((not self._link) or (not self._mcpars)):
            print " ERROR! no container in TES for linker with IDs or MCParticles"
        

    def linkKey(self,id):
        """ return the key of the MCParticle associated to this LHCbID
        """
        ok = self._link.firstReference(id.lhcbID(),self._Null,self._ref)
        if (not ok): return -1
        else:
            return self._ref.objectKey()
        
    def mcparticle(self,id):
        """ return the MCParticle associated to this LHCbID (none if none)
        """
        key = self.linkKey(id)
        if (key<0): return none
        par = self._mcpars[key]
        return par

    def linkKeys(self,ids):
        """ return a list of mcparticles keys associated to this list of LHCbIDs
        """
        keys = map(lambda x: self.linkKey(x),ids)
        return keys
        
    def mcparticles(self,ids):
        """ return a list of (mcparticle,weight) of the mcparticles associated (with weight) to
        this list of LHCbIDs
        """
        keys = self.linkKeys(ids)
        counts = map(lambda x: (keys.count(x),x),keys)
        counts.sort()
        mykeys = []
        mypars = []
        n0 = len(keys)
        for c in counts:
            key = c[1]
            if (key not in mykeys):
                mykeys.append(key)
                w = (1.*c[0])/(1.*n0)
                if (key>=0): mypars.append([self._mcpars[key],w])
                else: mypars.append([none,w])
                if (DEBUG): print " key ",key," w ",w
        return mypars

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

def eve(mcpar,i=0):
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
            (mom,i) = eve(mcpar,i)
            if ((mom.key() == iroot.key()) and (i <= ngenerations)):
                bsons.append(mcpar)
                bsignal.append(bsons)
    if (DEBUG): print " findDecay: size ",len(bsignal)    
    return bsignal


