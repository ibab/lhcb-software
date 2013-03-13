#This file holds utility functions for the swimming job

__all__ = ['getBinfo',
           'runSwimmingStep',
           'createObject',
           'getSelector']

import GaudiPython
GaudiPython.loaddict( 'SwimmingDict' )
Random = GaudiPython.Bindings.gbl.Random

def createObject(t, *args):
    """ Function to create objects which are not owned by python, will not be
    garbage collected and can therefore be put in the TES. """
    i = t(*args)
    import ROOT
    ROOT.SetOwnership(i, False)
    return i

class RandomSelector(object):
    def __init__(self, globs, extra = 'RandomSelector'):
        self._extra = extra
        self._initialRandom = Random.mixString(len(extra), extra)
        self._TES = globs.TES
 
    def __call__(self, candidates):
        odin = self._TES['DAQ/ODIN']
        r = self.random(odin)
        n = candidates.size()
        for i in xrange(n):
            if r <= (float(i) / float(n)): break
        return candidates.containedObjects()[i]

    def random(self, odin):
        x = self._initialRandom;
        x = Random.mix64(x, odin.gpsTime())
        x = Random.mix32(x, odin.runNumber())
        x = Random.mix64(x, odin.eventNumber())
        return float(x) / float(0xFFFFFFFF)

class FirstSelector(object):
    def __init__(self, *args):
        pass

    def __call__(self, candidates):
        return candidates.containedObjects()[0]
    
_selectors = {'random' : RandomSelector,
              'first'  : FirstSelector,
              'none'   : FirstSelector}

def getSelector(s):
    return _selectors[s]

#
#
#
#Grab the two properties we are most interested in
def getBinfo(myGlobs,mycand):
    if not myGlobs.swimStripping :
        return (myGlobs.gaudi.algorithm('HltMovePVs4Swimming')).Blifetime, (myGlobs.gaudi.algorithm('HltMovePVs4Swimming')).BipChi2
    else :
        temptimes = getcandpropertimeinfo(myGlobs,mycand)
        return (temptimes[0],temptimes[4])
#
#
#
#Get the info of the candidate for the ntuple
#in the case of swimming the stripping/offline
def getcandpropertimeinfo(myGlobs,mycand):
    #
    myrelatedPV = myGlobs.m_relatedPVTool.relatedPV(mycand,myGlobs.TES[myGlobs.offlinePVs])
    btimes = []
    #now the temporary ROOT rubbish
    import ROOT
    #First the lifetime
    tau     = ROOT.Double(-1000000.0)
    tauerr  = ROOT.Double(-1000000.0)
    tauchi2 = ROOT.Double(-1000000.0)
    myGlobs.m_lifetimeTool.fit(myrelatedPV,mycand,tau,tauerr,tauchi2)
    btimes.append(tau)
    btimes.append(tauerr)
    #Now the flight distance and ips
    fd      = ROOT.Double(-1000000.0)
    fdchi2  = ROOT.Double(-1000000.0)
    mip     = ROOT.Double(-1000000.0)
    mipchi2 = ROOT.Double(-1000000.0)
    myGlobs.m_distanceTool.distance(mycand,myrelatedPV,mip,mipchi2)
    myGlobs.m_distanceTool.distance(myrelatedPV,mycand.endVertex(),fd,fdchi2)
    btimes.append(fdchi2)
    btimes.append(mip)
    btimes.append(mipchi2)
    #Finally return the answer
    return btimes
#
#
#
#Move the PV, for swimming the stripping
def movePVs(myGlobs, mycand, swimLoop) :
    if myGlobs.DEBUGMODE :
        print "Old PV positions are"
        for PV in myGlobs.TES[myGlobs.offlinePVs] :
            print PV
    for PV in myGlobs.TES[myGlobs.offlinePVs]:
        PV.position().SetX(PV.position().X() + swimLoop * mycand.slopes().Unit().X())
        PV.position().SetY(PV.position().Y() + swimLoop * mycand.slopes().Unit().Y())
        PV.position().SetZ(PV.position().Z() + swimLoop * mycand.slopes().Unit().Z())    
    if myGlobs.DEBUGMODE :
        print "New PV positions are"
        for PV in myGlobs.TES[myGlobs.offlinePVs] :
            print PV
    return True
#
#
#                
#
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
    ## return myGlobs.TES[myGlobs.offCands+'/Particles'][0]
