#This file holds utility functions for the swimming job
from Configuration import Swimming
from collections import defaultdict
from GaudiKernel.PhysicalConstants import c_light

__all__ = ['getCandidateinfo',
           'runSwimmingStep',
           'hashParticle',
           'hashParticleNoCompositePID',
           'intHashParticle',
           'intHashParticleNoCompositePID',
           'matchParticles',
           'matchParticlesNoCompositePID',
           'createObject',
           'getSelector']

# Load Swimming dictionaries
import GaudiPython
GaudiPython.loaddict('SwimmingEventDict')
from GaudiPython.Bindings import gbl
hashParticle                  = gbl.Swimming.hashParticle
hashParticleNoCompositePID    = gbl.Swimming.hashParticleNoCompositePID
intHashParticle               = gbl.Swimming.intHashParticle
intHashParticleNoCompositePID = gbl.Swimming.intHashParticleNoCompositePID
matchParticles                = gbl.Swimming.matchParticles
matchParticlesNoCompositePID  = gbl.Swimming.matchParticlesNoCompositePID
import ROOT

def createObject(t, *args):
    """
    Function to create objects which are not owned by python.
    
    These objects will not be garbage collected and can therefore be put in the
    TES, which always takes ownership.
    """
    i = t(*args)
    ROOT.SetOwnership(i, False)
    return i

import pprint
from copy import copy

class Candidate(object):
    def __init__(self, particle, selection = None):
        if not particle:
            raise RuntimeError('must provide a particle')
        self._particle = particle
        self.__hash = None
        self.__inthash = None
        self.__ids = None
        self._selections = {}
        if not selection:
            return
        if type(selection) == str:
            self._selections[selection] = particle
        elif type(selection) == dict:
            self._selections = copy(selection)
        else:
            raise TypeError('Selection must be a string')

    def particle(self, selection = None):
        if selection:
            return self.selectedParticle(selection)
        else:
            return self.particles()[0]

    def particles(self):
        if self._selections:
            return self._selections.values()
        else:
            return [self._particle]

    def addSelection(self, sel, p = None):
        if sel in self._selections:
            raise KeyError('Selections %s is already present')
        if not p:
            self._selections[sel] = self._particle
        elif hashParticleNoCompositePID(p) != self.__hash__():
            raise RuntimeError('Attempting to add a particle with a different hash')
        self._selections[sel] = p

    def selected(self, sel):
        return sel in self._selections
            
    def selections(self):
        return self._selections.keys()
    
    def selectedParticle(self, sel):
        if sel in self._selections:
            return self._selections[sel]
        else:
            return None

    def selectedParticles(self):
        return self._selections
  
    def __hash__(self):
        if not self.__hash:
            self.__hash = hashParticleNoCompositePID(self.particle())#self.__hp()#self.particle())
            #print "Calculating hash:",self.__hash
        return self.__hash

    def intHash(self):
        if not self.__inthash:
            self.__inthash = intHashParticleNoCompositePID(self.particle())
        return self.__inthash

    def __nonzero__(self):
        return not not self.particle()

    def __eq__(self, other):
        if type(other) == type(self):
            return matchParticlesNoCompositePID(self.particle(), other.particle())#self.ids() == other.ids()
            #return hash(self) == hash(other)
        elif type(other) == GaudiPython.Bindings.gbl.LHCb.Particle:
            return matchParticlesNoCompositePID(self.particle(), other)#self.ids() == self.__ids(other)
            #return hash(self) == self.__hp(other)
        else:
            return False

    def __ne__(self, other):
        return not self.__eq__(other)

    def __getattr__(self, k):
        if k in self.__dict__:
            return self.__dict__[k]
        else:
            return getattr(self.particle(), k)

    def __str__(self):
        if not self._selections:
            s = str(self.particle())
        else:
            s = ''
            for loc, p in self._selections.iteritems():
                s += loc + '\n'
                s += format(p) + '\n'
        return s

class LifetimeFitter(object):
    def __init__(self, globs):
        self._globs = globs
        self._lifetimeTool = globs.gaudi.toolsvc().create(Swimming().getProp('TauCalc'),
                                                          interface = 'ILifetimeFitter')
        
    def lifetime(self, candidate):
        pv = self._globs.relatedPVFinder.relatedPV(candidate)
        if not pv:
            return {'tau' : -1e6, 'LTF_tau_error' : -1e6,
                    'LTF_chi2dof' : 1e6, 'LTF_StatusCode' : 515}
        tau     = ROOT.Double(-1000000.0)
        tauerr  = ROOT.Double(-1000000.0)
        tauchi2 = ROOT.Double(-1000000.0)
        sc = self._lifetimeTool.fit(pv, candidate, tau, tauerr, tauchi2)
        if sc.isFailure():
            return {'tau' : -1e6, 'LTF_tau_error' : -1e6,
                    'LTF_chi2dof' : 1e6, 'LTF_StatusCode' : sc.getCode()}
        else:
            # DoF of the lifetime fitter is always 2
            return {'tau' : tau, 'LTF_tau_error' : tauerr,
                    'LTF_chi2dof' : tauchi2 / 2., 'LTF_StatusCode' : sc.getCode()}

class DTFFitter(object):
    def __init__(self, globs):
        self._globs = globs
        self._dtfTool = globs.gaudi.toolsvc().create('LoKi::DecayTreeFit',
                                                     interface = 'IDecayTreeFit')

    def lifetime(self, candidate):
        pv = self._globs.relatedPVFinder.relatedPV(candidate)
        if not pv:
            return {'tau' : -1e6, 'DTF_sigma_tau' : -1e6,
                    'DTF_chi2dof' : 1e6, 'DTF_StatusCode' : 515}
        sc = self._dtfTool.fit(candidate, pv)
        if sc.isFailure():
            return {'tau' : -1e6, 'DTF_tau_error' : -1e6,
                    'DTF_chi2dof' : 1e6, 'DTF_StatusCode' : sc.getCode()}
        else:
            params = self._dtfTool.fitted(candidate)
            ctau = params.ctau()
            return {'tau' : ctau.value() / c_light,
                    'DTF_tau_error' : ctau.error() / c_light,
                    'DTF_chi2dof' : self._dtfTool.chi2() / self._dtfTool.nDoF(),
                    'DTF_StatusCode' : sc.getCode()}
        
class BestPVFinder(object):
    def __init__(self, globs):
        self.globs = globs
        config = Swimming()
        self.__refit = config.getProp('RefitPVs')
        toolType = config.getProp('RelPVFinder')
        self.__relatedPVTool = globs.gaudi.toolsvc().create(toolType, interface = 'IRelatedPVFinder')
        if not self.__relatedPVTool:
            raise RuntimeError, 'Could not get tool %s' % toolType

        if not self.__refit:
            return

        toolType = config.getProp('PVRefitter')
        self.__refitTool = globs.gaudi.toolsvc().create(toolType, interface = 'IPVReFitter')
        if not self.__refitTool:
            raise RuntimeError, 'Could not get tool %s' % toolType

    def relatedPV(self, particle):            
        # Find the best PV using the tool
        args = [particle]
        if self.__refit: args.append(self.__refitTool)
        pvs = self.globs.service.getOfflinePVs(*args)
        return self.__relatedPVTool.relatedPV(particle, pvs)

    def refitter(self):
        if self.__refit:
            return self.__refitTool
        else:
            return None

class RandomSelector(object):
    """
    Selector which deteministically selects a single offline candidate.

    This selector uses the code developed for the HLT's DeterministicPrescaler
    to randomly select a single offline candidate in the event. It uses as input
    (same as DeterministicPrescaler):
    - event GPS time
    - event run number
    - event number

    This ensures that in an event any container with size m will always result
    in a selected candidate with position in the container 0 <= n < m.
    """
    def __init__(self, globs, extra = 'RandomSelector'):
        self._extra = extra
        self._initialRandom = None
        self._random = None
        self._TES = globs.TES
 
    def __call__(self, candidates):
        odin = self._TES['DAQ/ODIN']
        r = self.random(odin)
        n = len(candidates)
        for i in xrange(n):
            if r <= (float(i) / float(n)): break
        return candidates[i]

    def random(self, odin):
        if self._random == None:
            import GaudiPython
            GaudiPython.loaddict( 'SwimmingDict' )
            self._random = GaudiPython.Bindings.gbl.Random
        if not self._initialRandom:
            self._initialRandom = self._random.mixString(len(self._extra), self._extra)
        x = self._initialRandom;
        x = self._random.mix64(x, odin.gpsTime())
        x = self._random.mix32(x, odin.runNumber())
        x = self._random.mix64(x, odin.eventNumber())
        return float(x) / float(0xFFFFFFFF)

class FirstSelector(object):
    """
    Selector which selects the first candidate in a container.
    """
    def __init__(self, *args):
        pass

    def __call__(self, candidates):
        return candidates[0]
    
_selectors = {'random' : RandomSelector,
              'first'  : FirstSelector,
              'none'   : FirstSelector}

def getSelector(s):
    return _selectors[s]

#Get the info of the candidate for the ntuple
#in the case of swimming the stripping/offline
def getCandidateInfo(globs, candidate):
    # TODO understand this:
    # if not myGlobs.swimStripping :
    #     return (myGlobs.gaudi.algorithm('HltMovePVs4Swimming')).Blifetime, (myGlobs.gaudi.algorithm('HltMovePVs4Swimming')).BipChi2
    # else :
    #     return stuff like below
    # end TODO

    relatedPV = globs.relatedPVFinder.relatedPV(candidate) # TODO check the extra layer of abstraction...
    #now the temporary ROOT rubbish
    info = globs.lifetimeFitter.lifetime(candidate)
    #Now the flight distance and ips
    fd      = ROOT.Double(-1000000.0)
    fdchi2  = ROOT.Double(-1000000.0)
    mip     = ROOT.Double(-1000000.0)
    mipchi2 = ROOT.Double(-1000000.0)
    globs.distanceTool.distance(candidate, relatedPV, mip, mipchi2)
    globs.distanceTool.distance(relatedPV, candidate.endVertex(), fd, fdchi2)
    info['fdchi2'] = fdchi2
    info['mip'] = mip
    info['mipchi2'] = mipchi2
    #Finally return the answer
    return info

#Move the PV, for swimming the stripping
def movePVs(globs, particle, swimLoop) :
    globs.service.setOffset(swimLoop * particle.slopes().Unit())
    if globs.DEBUGMODE:
        print "New offline PV positions are"
        for PV in globs.service.getOfflinePVs(particle):
            print PV
    return True

def runSwimmingStep(myGlobs, mycand, swimLoop):
    myGlobs.TES["/Event/Rec/ProtoP/Charged"]
    myGlobs.TES["/Event/Rec/ProtoP/Neutrals"]
    myGlobs.TES[myGlobs.offlinePVs]
    if myGlobs.swimStripping:
        myGlobs.incidentSvc.fireIncident(myGlobs.GaudiPython.gbl.Incident("VavaIncident","EndEvent"))
        myGlobs.gaudi.executeEvent()
        myGlobs.gaudi.algorithm("killStrippingSeq").execute()
        if myGlobs.DEBUGMODE:
            "About to move the PVs for the stripping"
        movePVs(myGlobs, mycand, swimLoop)
        myGlobs.gaudi.algorithm("StrippingGlobal").execute()
    else :
        myGlobs.incidentSvc.fireIncident(myGlobs.GaudiPython.gbl.Incident("VavaIncident","EndEvent"))
        myGlobs.gaudi.executeEvent()
        myGlobs.gaudi.algorithm("HltMovePVs4Swimming").SwimmingDistance = swimLoop
        myGlobs.gaudi.algorithm("killHltSeq").execute()
        myGlobs.gaudi.algorithm("seqL0").execute() 
        myGlobs.gaudi.algorithm("Hlt").execute()
        # Since the offline PVs are used to calculate the candidate liftime at
        # the turning point, these need to be moved too.
        movePVs(myGlobs, mycand, swimLoop)
