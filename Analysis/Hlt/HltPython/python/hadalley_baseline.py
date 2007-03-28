#! /usr/bin/env python
import gaudimodule
from ROOT import TH1F,TH2F

gaudimodule.loaddict("L0EventDict")
gaudimodule.loaddict("HltBaseDict")

from eventassoc import linkedTo

import mctools
from ParticleTranslator import PTranslate

HOME= "/afs/cern.ch/user/h/hernando/w0/DC06/Moore_v2r1"
JOBOPTS = HOME+"/Hlt/Moore/v2r1/options/Moore_HadL0Conf_rawmb.opts"

gaudi = gaudimodule.AppMgr(outputlevel=3)
gaudi.config( files = [JOBOPTS,])

DO_MC = False
MC_GEN = 3
# MC_GEN = 3
DEBUG = False
VERBOSE = False
VVERBOSE = False
DO_HISTOS = True

NEVENTS = 10

TES = gaudi.evtsvc()

State = gaudimodule.gbl.LHCb.State
Track = gaudimodule.gbl.LHCb.Track
MCParticle = gaudimodule.gbl.LHCb.MCParticle
VectorTracks = gaudimodule.gbl.std.vector("LHCb::Track *")

__DOCA = None
if (not DO_MC):
    DOCA = gaudimodule.gbl.Hlt.DOCA
    __DOCA = DOCA()
    print " creating DOCA func ",__DOCA
    FC = gaudimodule.gbl.Hlt.FC

LODU_CALOELE   = 0
L0DU_CALOGAMMA = 1
LODU_CALOHAD   = 2
LODU_CALOPI0   = 3
LODU_CALOPI0GLOBAL   = 4
LODU_CALOSUMET   = 5
L0DU_CALOPSDMULT = 6

L0DUCHANNEL_HAD = 9

TRACKTYPE_LONG = 3
TRACKFLAG_BACKWARD = 1
TRACKINFO_IP = 10

Z_MAGNET = 5250. #mm
K_MAGNET = 1263. #MeV

CUT_ET = 3500. # MeV
CUT_CHI2 = 5.
CUT_IPMIN = 0.
CUT_IPMAX = 1e6

RATES = ["Entries","NMC",
         "L0","L0Had",
         "PreTrig:NIP:RZVelo",
         "PreTrig:N:Velo",
         "PreTrig:NIP:Velo",
         "PreTrig:N:VeloTT",
         "PreTrig:NSingle",
         "PreTrig:NDiHad",
         "PreTrig:N",
         "Trig:NIP:Velo",
         "Trig:N:Forward",
         "Trig:NPt:Forward",
         "Trig:NDOCA:Vertex"
         ]

COUNTER = {}
for key in RATES: COUNTER[key] = 0

AVES = {}
for key in RATES: AVES[key] = 0

def rates(keys = RATES, norma = 1.):
    for key in keys:
        ave = None
        if (AVES.has_key(key) and COUNTER[key]>0):
            ave = float(AVES[key])/float(COUNTER[key])
        print key,norma*COUNTER[key],ave
    

HISTOS = {}
TOOLS  = {}

def _DOCA(x,y):
    if (DO_MC): return 0.1
    return __DOCA(x,y)

# _DOCA = DOCA()
# _FC   = FC()

#------------------- initialize -------------------------

def book():

    if (not DO_HISTOS): return

    def hbook(key,n,x0,xf,title=""):
        if (title == ""): title = key
        HISTOS[key] = TH1F(key,title,n,x0,xf)

    def hbook2(key,nx,x0,xf,ny,y0,yf,title=""):
        if (title == ""): title = key
        HISTOS[key] = TH2F(key,title,nx,x0,xf,ny,y0,yf)

    for key in RATES:
        hbook(key,20,0,20)

    hbook("MC:Pt",100,0,6000)
    hbook("MC:Pt1",100,0,6000)
    hbook("PreTrig:IP:RZVelo",100,-0.5,0.5)
    hbook("Trig:IP:Velo",100,-0.5,0.5)
    hbook("PreTrig:Pt:VeloTT",100,0,6000)
    hbook("PreTrig:Pt1:VeloTT",100,0,6000)
    hbook("Trig:Pt:Forward",100,0,6000)
    hbook("Trig:Pt1:Forward",100,0,6000)
    hbook("Trig:DOCA:Vertex",100,0,1.)

def hfill(name,x,w=1.):
    if (not DO_HISTOS): return
    HISTOS[name].Fill(x,w)

def tools():

    tool1 = gaudi.toolsvc().create("PrepareMuonSeed",interface="IPrepareMuonSeed")
    TOOLS["PrepareMuonSeed"] = tool1

    tool2 = gaudi.toolsvc().create("PrepareHadronSeed",interface="IPrepareCaloSeed")
    TOOLS["PrepareCaloSeed"] = tool2

    tool3 = gaudi.toolsvc().create("TsaConfirmTool",interface="ITrackConfirmTool")
    TOOLS["TsaL0Conf"] = tool3

    tool4 = gaudi.toolsvc().create("HltVeloRZL0HadCaloMatchTool",interface="ITrackL0CaloMatchTool")
    TOOLS["RZVeloL0Conf"] = tool4

    tool5 = gaudi.toolsvc().create("HltVeloL0HadCaloMatchTool",interface="ITrackL0CaloMatchTool")
    TOOLS["VeloL0Conf"] = tool5

    tool6 = gaudi.toolsvc().create("HltDataStore",interface="IHltDataStore")
    TOOLS["HltDataStore"] = tool6

book()
tools()

#------------------- execute -------------------------

def sort_pt(t1,t2):
    if (t1.pt() > t2.pt()): return -1
    elif (t1.pt() < t2.pt()): return +1
    return 0

def longtracks():
    ltracks = []
    tracks = TES["Rec/Track/Best"]
    if (not tracks): return ltracks
    def isLong(track):
        return track.checkType(TRACKTYPE_LONG)
    ltracks = filter(isLong,tracks)
    ltracks.sort(sort_pt)
    return ltracks

def hadcalos():
    """ return l0 calo candidates above a given cut in energy
    """
    hcalos = []
    calos = TES["Trig/L0/FullCalo"]
    if (not calos): return hcalos
    for calo in calos:
        if ((calo.type() == 2) and (calo.et() >= CUT_ET)):
            hcalos.append(calo)
    def sort_et(t1,t2):
        if (t1.et() > t2.et()): return -1
        elif (t1.et() < t2.et()): return +1
        return 0
    if (DEBUG):
        print " calos ",calos.size(), " had calos ", len(hcalos)
    hcalos.sort(sort_et)
    return hcalos

def signal_tracks(tracks,mcpars,link):
    """ MC: return the tracks linked with MC Bs
    """
    keys = map(lambda x: x.key(),mcpars)
    def isSignal(track):
        mcpar = link.first(track)
        if (not mcpar): return False
        return (mcpar.key() in keys)
    stracks = filter(isSignal,tracks)
    return stracks

def hasmc():

    out = []
    if (not DO_MC): return True,out

    mompid = PTranslate("B0s")
    sonspid = map(PTranslate,["pi+","pi-"])
    if (MC_GEN>1):
        sonspid = map(PTranslate,["pi+","pi-","K+","K-"])
    if (DEBUG): print " mon pid ",mompid," sons pid ",sonspid

    tracks = TES["Hlt/Track/Forward"]
    if (not tracks): return False
    if (tracks.size()<=0): return False,out
    link = linkedTo(MCParticle,Track,"Hlt/Track/Forward")
    if (not link): return False,out

    def isSignal(track):
        mcpar = link.first(track)
        if (not mcpar): return False
        (mon,i) = mctools.rootMother3(mcpar)
        if ( i > MC_GEN): return False
        if (abs(mon.particleID().pid()) != abs(mompid)): return False
        pid = mcpar.particleID().pid()
        return (pid in sonspid)
    
    stracks = filter(isSignal, tracks)
    n = len(stracks)
    if (DEBUG): print " number of matched tracks ",n

    ok = decision(" mc tracks ",n,"NMC",2)
    if (ok):
        pts = map(lambda x: x.pt(),stracks)
        pt1 = max(pts)
        for pt in pts: hfill("MC:Pt",pt,1.)
        hfill("MC:Pt1",pt1,1.)
        if (DEBUG): print " MC pts ",pts
        if (DEBUG): print " MC pt1 ",pt1
    return ok,stracks

def hadl0():
    """ select l0-yes and l0-hadron-yes and update counters
    """
    
    l0 = TES["Trig/L0/L0DU"]
    if (not l0): return False
    if (DEBUG): print " l0 ",l0.decision()
    if (not l0.decision()): return False
    COUNTER["L0"] += 1    
    
    if (DEBUG): print " l0 had ",l0.channelDecision(L0DUCHANNEL_HAD)
    if (not l0.channelDecision(L0DUCHANNEL_HAD)): return False
    COUNTER["L0Had"] += 1    

    return True


def matchIDs(t0,t1):
    """ return true if the ids of the 1st track (t0) are in (t1), 70% of them
    """
    ids1 = t0.lhcbIDs()
    ids2 = t1.lhcbIDs()
    n = 0
    for id1 in ids1:
        for id2 in ids2:
            if (id2 == id1): n += 1
    r = float(n)/float(len(ids1))
    if (VVERBOSE): print " matchIDs 1,2,common ",len(ids1),len(ids2),n,r
    return (r>=0.70)

def follow_tracks(tracks0,tracks1):
    """ returns tracks in path that were ancestors are in the list tracks
    """
    otracks = []
    for track1 in tracks1:
        ok = False
        for track0 in tracks0:
            if matchIDs(track0,track1): ok = True
        if (ok): otracks.append(track1)
    if (DEBUG): print " follow tracks: original, next and ancestors",len(tracks0),len(tracks1),len(otracks)
    return otracks

def backfollow_tracks(tracks0,tracks1, isin = True):
    """ returns tracks in path that were ancestors are in the list tracks
    """
    otracks = []
    for track0 in tracks0:
        ok = False
        for track1 in tracks1:
            if matchIDs(track0,track1): ok = True
        if (not isin): ok = not ok 
        if (ok): otracks.append(track0)
    if (DEBUG): print " backfollow tracks: original, next and ancestors",len(tracks0),len(tracks1),len(otracks)
    return otracks


def filter_ipcut(track, ipmin = 0., ipmax = 1e6, doabs = False):
    """ filter function to check if the ip of a track is in a window
    Assumes IP is located in the tracks as info
    """
    tip = track.info(TRACKINFO_IP,ipmax)
    if (doabs): tip = abs(tip)
    if (VVERBOSE): print tip
    return ( (tip>=ipmin) and (tip < ipmax) )

def filter_pt(track,ptmin = 0.):
    return (track.pt() >= ptmin)    

def decision(comment,n,hname, nmin = 1):
    if (DEBUG): print comment,n
    if (n<nmin): return False
    hfill(hname,1.*n,1.)
    COUNTER[hname] +=1
    AVES[hname] += n
    return True

#------------   algorithm -------------------

def algo_pretrigger(stracks):

    rIPmin =  0.1
    rIPmax =  3.
    IPmin =  0.15
    IPmax =  3.
    PT1   = 2550.
    PT21  = 1150.
    PT22  =  850.

    out = []
    
    # radial IP cut
    rzvelos = TES["Rec/Track/RZVelo"]
    if (DO_MC):
        rzvelos = backfollow_tracks(rzvelos,stracks)
    
    if (DEBUG): print " rzvelo tracks ",len(rzvelos)
    rzvelos = filter(lambda x: (not x.checkFlag(TRACKFLAG_BACKWARD)),rzvelos)
    if (DEBUG): print " rzvelo no-backwards tracks ",len(rzvelos)
    ips = map(lambda x: x.info(TRACKINFO_IP,-0.3),rzvelos)
    if (VERBOSE): print ips
    for ip in ips: hfill("PreTrig:IP:RZVelo",ip,1.)
    rzvelos = filter(lambda x: filter_ipcut(x,ipmin=rIPmin,ipmax=rIPmax),rzvelos)

    n = len(rzvelos)
    ok = decision(" ip rzvelos ", n, "PreTrig:NIP:RZVelo")
    if (not ok): return False,out
    

    # 3D IP cut
    velos = TES["Hlt/Track/Velo"]
    if (DEBUG): print " velo tracks ",len(velos)
    velos = follow_tracks(rzvelos,velos)

    n = len(velos)
    ok = decision(" velos ", n, "PreTrig:N:Velo")
    if (not ok): return False,out

    ips = map(lambda x: x.info(TRACKINFO_IP,-0.3),velos)
    if (VERBOSE): print ips
    velos = filter(lambda x: filter_ipcut(x,ipmin=IPmin,ipmax= IPmax),velos)

    n = len(velos)
    ok = decision(" ip velos ", n, "PreTrig:NIP:Velo")
    if (not ok): return False,out

    velotts = TES["Hlt/Track/VeloTT"]
    velotts = follow_tracks(velos,velotts)
    n = len(velotts)
    ok = decision(" velotts ", n, "PreTrig:N:VeloTT")
    if (not ok): return False,out

    # single line
    pts = map(lambda x: x.pt(),velotts)
    pt1 = max(pts)
    for pt in pts: hfill("PreTrig:Pt:VeloTT",pt,1.)
    hfill("PreTrig:Pt1:VeloTT",pt1,1.)
    if (DEBUG): print " pt1 ",pt1
    if (DEBUG): print " pts ",pts    

    spts = filter(lambda x: x>PT1,pts)
    ns = len(spts)
    ok1 = decision(" pretrigger single line ",ns,"PreTrig:NSingle")

    dpts = filter(lambda x: x>PT21,pts)
    nd = len(dpts)
    if (nd>0):
        dpts = filter(lambda x: x>PT22,pts)
        nd = len(dpts)
    ok2 = decision(" pretrigger di line ",nd,"PreTrig:NDiHad",2)

    n = ns
    if (ok2): n += nd
    ok = decision(" pretrigger ",n,"PreTrig:N")

    return ok,velotts

def algo_trigger(stracks):

    IPmin = 0.1
    IPmax = 3.
    DO_IPABS = True
    PTmin = 1000.
    DOCAmax = 0.2

    out = []

    velos = TES["Hlt/Track/Velo"]
    if (DO_MC):
        velos = backfollow_tracks(velos,stracks)

    if (DEBUG): print " velo tracks ",len(velos)
    velos = filter(lambda x: (not x.checkFlag(TRACKFLAG_BACKWARD)),velos)
    if (DEBUG): print " velo no-backwards tracks ",len(velos)
    ips = map(lambda x: x.info(TRACKINFO_IP,-0.3),velos)
    for ip in ips: hfill("Trig:IP:Velo",ip,1.)
    if (VERBOSE): print ips
    velos = filter(lambda x: filter_ipcut(x,ipmin= IPmin,ipmax= IPmax,doabs=True),velos)

    n = len(velos)
    ok = decision(" trigger ip velos ", n, "Trig:NIP:Velo")
    if (not ok): return False,out
    
    aforwards = TES["Hlt/Track/Forward"]
    if (DEBUG): print " forward tracks ",len(aforwards)
    forwards = follow_tracks(velos,aforwards)

    n = len(forwards)
    ok = decision(" trigger forward ", n, "Trig:N:Forward")
    if (not ok): return False,out

    pts = map(lambda x: x.pt(),forwards)
    pt1 = max(pts)
    for pt in pts: hfill("Trig:Pt:Forward",pt,1.)
    hfill("Trig:Pt1:Forward",pt1,1.)
    if (DEBUG): print " pt1 ",pt1
    if (DEBUG): print " pts ",pts
    
    forwards = filter(lambda x: filter_pt (x,PTmin),forwards)
    n = len(forwards)
    ok = decision(" trigger forward pt ", n, "Trig:NPt:Forward")
    if (not ok): return False,out

    pairs = []
    for i in range(len(forwards)):
        ftrack = forwards[i]
        for j in range(i+1,len(forwards)):
            vtrack = forwards[j]
            if (ftrack.key() == vtrack.key()): continue
            mdoca = _DOCA(ftrack,vtrack)
            hfill("Trig:DOCA:Vertex",mdoca,1.)
            if (mdoca < DOCAmax):
                pairs.append( [ftrack,vtrack]) 
            if (VERBOSE): print " pt pt doca ",ftrack.pt(),vtrack.pt(),mdoca

    n = len(pairs)
    ok = decision(" trigger good doca vertices ",n,"Trig:NDOCA:Vertex")
    if (not ok): return False,out

    return True,forwards

              
def hadalley_algo():

    COUNTER["Entries"] += 1
    if (DEBUG): print " ************* "

    ok,stracks = hasmc()
    if (not ok): return False

    ok = hadl0()
    if (not ok): return False

    ok,velotts = algo_pretrigger(stracks)
    if (not ok): return False

    ok,forwards = algo_trigger(stracks)
  
    return True

def go(n=1):
    i = 0
    while(i<n):
        gaudi.run(1)
        velol0conf_algo()
        i =i+1

def next():
    ok = False
    while(not ok):
        gaudi.run(1)

class HadAlley(gaudimodule.PyAlgorithm):
    def execute ( self ) :
        hadalley_algo()
        return True
    


alg = HadAlley("HadtAlley")
gaudi.addAlgorithm (alg )
print "------------ Here we are ------------ "
gaudi.run(NEVENTS)

#-----------------------------------------

def ncandis(keys = RATES[3:]):
    for key in keys:
        print key,HISTOS[key].GetEntries(),HISTOS[key].GetMean(),HISTOS[key].GetRMS()
    
def makeList(cobjs):
    out = []
    for i in range(cobjs.size()):
        obj = cobjs.containedObject(i)
        out.append(obj)
    return obj 
    
print rates()

def makeList(cobjs):
    out = []
    for i in range(cobjs.size()):
        obj = cobjs.containedObject(i)
        if (obj): out.append(obj)
    return out

def mTES(name):
    cobjs = TES[name]
    if (not cobjs): return []
    return makeList(cobjs)
