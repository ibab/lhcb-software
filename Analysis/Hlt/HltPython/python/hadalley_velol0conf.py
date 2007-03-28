#! /usr/bin/env python
import gaudimodule
from ROOT import TH1F,TH2F

gaudimodule.loaddict("L0EventDict")
# gaudimodule.loaddict("HltEventDict")
gaudimodule.loaddict("HltBaseDict")

from eventassoc import linkedTo

import mctools
from ParticleTranslator import PTranslate

HOME= "../options"
JOBOPTS = HOME+"/Moore_HadL0Conf_rawmb.opts"

gaudi = gaudimodule.AppMgr(outputlevel=3)
gaudi.config( files = [JOBOPTS,])

DEBUG = True
VERBOSE = True
VVERBOSE = False
DO_MC = False
DO_HISTOS = True
DO_IPABS = False

NEVENTS = 10

TES = gaudi.evtsvc()

State = gaudimodule.gbl.LHCb.State
Track = gaudimodule.gbl.LHCb.Track
MCParticle = gaudimodule.gbl.LHCb.MCParticle
VectorTracks = gaudimodule.gbl.std.vector("LHCb::Track *")

## __DOCA = None
## if (not DO_MC):
##     DOCA = gaudimodule.gbl.Hlt.DOCA
##     __DOCA = DOCA()
##     print " creating DOCA func ",__DOCA
##     FC = gaudimodule.gbl.Hlt.FC

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
         "L0Conf:NHadCalos",
         "L0Conf:NIP:RZVelo",
         "L0Conf:NGoodChi2:RZVelo",
         "L0Conf:NIP:Velo",
         "L0Conf:NGoodChi2:Velo",
         "L0Conf:NIP:Forward",
         "L0Conf:NPt:Forward",
         "DiHad:NIPExtra:RZVelo",
         "DiHad:NIPExtra:Velo",
         "DiHad:NDOCAExtra:Velo",
         "DiHad:NExtra:Forward",
         "DiHad:NDOCA:Vertex",
         "DiHad:NPt:Vertex"
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
    return 0.1
    # if (DO_MC): return 0.1
    # return __DOCA(x,y)

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
    hbook("L0Conf:IP:RZVelo",100,-0.5,0.5)
    hbook("L0Conf:Chi:RZVelo",100,0,50.)
    hbook("L0Conf:Chi1:RZVelo",100,0,50.)
    hbook("L0Conf:Chi:Velo",100,0,50.)
    hbook("L0Conf:Chi1:Velo",100,0,50.)
    hbook("L0Conf:Pt:Forward",100,0,6000)
    hbook("L0Conf:Pt1:Forward",100,0,6000)
    hbook("DiHad:DOCAExtra:Velo",100,0,1.)
    hbook("DiHad:PtExtra:Forward",100,0,6000.)
    hbook("DiHad:Pt1Extra:Forward",100,0,6000.)
    hbook("DiHad:DOCA:Vertex",100,0,1.)
    hbook("DiHad:Pt:Vertex",100,0,6000.)
    hbook("DiHad:Pt1:Vertex",100,0,6000.)

    # --- temp studies
    hbook("Ana:Pt1NoChi:RZVelo",100,0.,6000.)

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
        if ( i > 3): return False
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
        if (DEBUG): print " MC pts ",pts.sort()
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

def conf(stracks,hcalos,tool,hname = None):
    """ match tracks and l0-calo-candidates with a chi2cut using a matching tool
    returns a list with xhi2 (track,calo) entries
    """
    Chimax = 5.
    gchis = []
    chi = 1e6
    if (len(stracks)<=0): return chi,gchis
    if (len(hcalos)<=0): return chi,gchis
    def chitrack(track):
        chi2 = map(lambda x: [tool.match(track,x),x],hcalos)
        if (hname):
            for c in chi2: hfill(hname,c[0],1.)
        chical = min(chi2)
        return [chical[0],[track,chical[1]]]
    chis = map(chitrack,stracks)
    chi = min(chis)
    gchis = filter(lambda x: (x[0]< Chimax),chis)
    gchis.sort()
    # if (DEBUG): print " chi ",chi, " good chi ",gchis
    return chi[0],gchis

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


def filter_ipcut(track,ipmin = 0., ipmax = 1e6, doabs=False):
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

def algo_l0conf(hcalos,stracks):

    IPmin =  0.05
    IPmax =  3.

    out = []
    
    # radial IP cut
    rzvelos = []
    rzvelos0 = TES["Rec/Track/RZVelo"]
    if (DO_MC):
        rzvelos = backfollow_tracks(rzvelos,stracks)
    else:
        rzvelos = rzvelos0
    
    if (DEBUG): print " rzvelo tracks ",len(rzvelos)
    rzvelos1 = filter(lambda x: (not x.checkFlag(TRACKFLAG_BACKWARD)),rzvelos)
    if (DEBUG): print " rzvelo no-backwards tracks ",len(rzvelos1)
    ips = map(lambda x: x.info(TRACKINFO_IP,-0.3),rzvelos1)
    for ip in ips: hfill("L0Conf:IP:RZVelo",ip,1.)
    ips.sort()
    if (VERBOSE): print " ips ",ips
    rzvelos2 = filter(lambda x: filter_ipcut(x,ipmin= IPmin,ipmax= IPmax),rzvelos1)

    n = len(rzvelos2)
    ok = decision(" ip rzvelos ", n, "L0Conf:NIP:RZVelo")
    if (not ok): return False,out
    
    # radial l0 match
    tool = TOOLS["RZVeloL0Conf"]
    rzchi,rzgchis = conf(rzvelos2,hcalos,tool,"L0Conf:Chi:RZVelo")
    hfill("L0Conf:Chi1:RZVelo",rzchi,1.)
    if (DEBUG): print " rzvelo chi-event ",rzchi, " n good chi2 ",len(rzgchis)
    rzvelos3 = map(lambda x: x[1][0],rzgchis)

    n = len(rzvelos3)
    ok = decision(" ip rzvelos good match", n, "L0Conf:NGoodChi2:RZVelo")

    def ana_noconf():
        if (not DO_MC): return
        pts = map(lambda x: x.pt(),stracks)
        pt1 = max(pts)
        hfill("Ana:Pt1NoChi:RZVelo",pt1,1.)
    if (not ok): ana_noconf()
    if (not ok): return False,out

    # 3D IP cut
    velos0 = TES["Hlt/Track/Velo"]
    if (DEBUG): print " velo tracks ",len(velos0)
    velos1 = follow_tracks(rzvelos3,velos0)

    n = len(velos1)
    ok = decision(" ip velos ", n, "L0Conf:NIP:Velo")
    if (not ok): return False,out

    # 3D l0 match
    tool = TOOLS["VeloL0Conf"]
    chi,gchis = conf(velos1,hcalos,tool,"L0Conf:Chi:Velo")
    hfill("L0Conf:Chi1:Velo",chi,1.)
    if (DEBUG): print " velo chi-event ",chi, " n good chi2 ",len(gchis)
    velos2 = map(lambda x: x[1][0],gchis)
    hads2  = map(lambda x: x[1][1],gchis)

    n = len(velos2)
    ok = decision(" ip velos good match", n, "L0Conf:NGoodChi2:Velo")
    if (not ok): return False,out

    return True,velos2

def algo_pretrigger(velos):

    PTmin = 2000.

    out = []
    
    aforwards = TES["Hlt/Track/Forward"]
    if (DEBUG): print " forward tracks ",len(aforwards)
    forwards = follow_tracks(velos,aforwards)

    n = len(forwards)
    ok = decision(" ip forward ", n, "L0Conf:NIP:Forward")
    if (not ok): return False,out

    pts = map(lambda x: x.pt(),forwards)
    for pt in pts: hfill("L0Conf:Pt:Forward",pt,1.)
    pt1 = max(pts)
    if (DEBUG): print " pt1 ",pt1
    pts.sort()
    if (DEBUG): print " pts ",pts
    hfill("L0Conf:Pt1:Forward",pt1,1.)
    
    forwards1 = filter(lambda x: filter_pt(x,PTmin),forwards)
    n = len(forwards1)
    ok = decision(" ip forward good pt", n, "L0Conf:NPt:Forward")
    if (not ok): return False,out

    return True,forwards1

def dihad_extratracks(forwards,stracks):

    IPmin = 0.1
    IPmax = 3.0

    out = []
    
    # radial IP cut
    rzvelos =[]
    rzvelos0 = TES["Rec/Track/RZVelo"]
    if (DO_MC):
        rzvelos = backfollow_tracks(rzvelos0,stracks)
    else:
        rzvelos = rzvelos0
    if (DEBUG): print " backfollow signal tracks in rzvelo ",len(rzvelos)

    rzvelos1 = backfollow_tracks(rzvelos,forwards,False)
    if (DEBUG): print " backfollow rzvelo not in pretrigger ",len(rzvelos1)
    
    if (DEBUG): print " rzvelo tracks ",len(rzvelos1)
    rzvelos2 = filter(lambda x: (not x.checkFlag(TRACKFLAG_BACKWARD)),rzvelos1)
    if (DEBUG): print " rzvelo no-backwards tracks ",len(rzvelos2)
    ips = map(lambda x: x.info(TRACKINFO_IP,-0.3),rzvelos2)
    ips.sort()
    if (VERBOSE): print " ips ",ips
    rzvelos3 = filter(lambda x: filter_ipcut(x,ipmin=IPmin,ipmax=IPmax),rzvelos2)

    n = len(rzvelos3)
    ok = decision(" dihad ip rzvelos ", n, "DiHad:NIPExtra:RZVelo")
    if (not ok): return False,out
    
    # 3D IP cut
    velos0 = TES["Hlt/Track/Velo"]
    if (DEBUG): print " velo tracks ",len(velos0)
    velos = follow_tracks(rzvelos,velos0)

    n = len(velos)
    ok = decision(" dihad ip velos ", n, "DiHad:NIPExtra:Velo")
    if (not ok): return False,out

    def filter_doca(vtrack, mindoca = 0.2):
        docas = map(lambda x: _DOCA(vtrack,x),forwards)
        for d in docas: hfill("DiHad:DOCAExtra:Velo",d,1.)
        mdoca = min(docas)
        if (VERBOSE): print " min doca ",mdoca
        return (mdoca<mindoca)

    velos1 = filter(filter_doca,velos)

    n = len(velos1)
    ok = decision(" dihad ip-doca velos ", n, "DiHad:NDOCAExtra:Velo")
    if (not ok): return False,out

    aforwards = TES["Hlt/Track/Forward"]
    vforwards = follow_tracks(velos1,aforwards)
    if (DEBUG): print " velo follow forward ",len(vforwards)
    def filter_isConfirmed(ftrack):
        ids = map(lambda x: matchIDs(x,ftrack),forwards)
        return (max(ids)==1)
    vforwards1 = filter(lambda x: not filter_isConfirmed(x),vforwards)

    n = len(vforwards1)
    ok = decision(" velo follow non-conf ",n,"DiHad:NExtra:Forward")
    if (not ok): return ok,out

    pts = map(lambda x: x.pt(),vforwards1)
    for pt in pts: hfill("DiHad:PtExtra:Forward",pt,1.)
    pt1 = max(pts)
    pts.sort()
    if (DEBUG): print " pts ",pts
    if (DEBUG): print " pt1 ",pt1
    hfill("DiHad:Pt1Extra:Forward",pt1,1.)
    
    return True,vforwards1

def algo_dihad(forwards,stracks):

    DOCAmin = 0.2
    PTmin  = 1000.
    out = []

    ok,vtracks0 = dihad_extratracks(forwards,stracks)
    
    vtracks = []
    for t in forwards: vtracks.append(t)
    for t in vtracks0: vtracks.append(t)
    if (DEBUG): print " vertex: confirmed tracks ",len(forwards)
    if (DEBUG): print " vertex: total     tracks ",len(vtracks) 
   
    pairs = []
    for i in range(len(forwards)):
        ftrack = forwards[i]
        for j in range(i+1,len(vtracks)):
            vtrack = vtracks[j]
            if (ftrack.key() == vtrack.key()): continue
            mdoca = _DOCA(ftrack,vtrack)
            hfill("DiHad:DOCA:Vertex",mdoca,1.)
            if (mdoca < DOCAmin):
                pairs.append( [ftrack,vtrack]) 
            if (VERBOSE): print " pt pt doca ",ftrack.pt(),vtrack.pt(),mdoca

    n = len(pairs)
    ok = decision(" good doca vertices ",n,"DiHad:NDOCA:Vertex")
    if (not ok): return False,out

    pts = map(lambda x: x[1].pt(),pairs)
    pt1 = max(pts)
    pts.sort()
    hfill("DiHad:Pt1:Vertex",pt1)
    if (DEBUG): print " pt1 ",pt1
    if (DEBUG): print " pts ",pts

    def filter_vertex(x):
        return (x[1].pt() > PTmin)
    pairs1 = filter(filter_vertex,pairs)
    n = len(pairs1)

    ok = decision(" good pt vertices",n,"DiHad:NPt:Vertex")
    if (not ok): return ok,out
        
    return True,vtracks
    
def velol0conf_algo():

    COUNTER["Entries"] += 1
    if (DEBUG): print " ************* "

    ok,stracks = hasmc()
    if (not ok): return False

    ok = hadl0()
    if (not ok): return False

    hcalos = hadcalos()
    n = len(hcalos)
    ok = decision(" had calos ",n,"L0Conf:NHadCalos")
    if (not ok): return False

    ok,velos = algo_l0conf(hcalos,stracks)
    if (not ok): return False

    ok,forwards = algo_pretrigger(velos)
    if (not ok): return False

    ok,vforwards = algo_dihad(forwards,stracks)
  
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

class VeloL0Conf(gaudimodule.PyAlgorithm):
    def execute ( self ) :
        velol0conf_algo()
        return True
    


alg = VeloL0Conf("VeloL0Conf")
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
