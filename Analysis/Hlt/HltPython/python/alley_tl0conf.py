#! /usr/bin/env python

# import gaudi and ROOT
import gaudimodule
from ROOT import TH1F,TH2F

# load extra dictionaires
gaudimodule.loaddict("L0EventDict")
gaudimodule.loaddict("HltBaseDict")

# load extra modules for MC
from eventassoc import linkedTo
import mctools
from ParticleTranslator import PTranslate

# location of the opts to run
HOME= "../options"
JOBOPTS = HOME+"/Moore_TL0Conf_rawmb.opts"

# create a gaudi application from a list of opts files
gaudi = gaudimodule.AppMgr(outputlevel=3)
gaudi.config( files = [JOBOPTS,])

# global control variables
DEBUG = True
VERBOSE = True
VVERBOSE = False
DO_MC = False
DO_HISTOS = True

NEVENTS = 1

#---------  global definitions ------------------------

# get the TES from gaudi
TES = gaudi.evtsvc()

# shortcuts for some LHCb data types
State = gaudimodule.gbl.LHCb.State
Track = gaudimodule.gbl.LHCb.Track
MCParticle = gaudimodule.gbl.LHCb.MCParticle
VectorTracks = gaudimodule.gbl.std.vector("LHCb::Track *")

# TODO: DOCA function not in Dictionary!
## __DOCA = None
## if (not DO_MC):
##     DOCA = gaudimodule.gbl.Hlt.DOCA
##     __DOCA = DOCA()
##     print " creating DOCA func ",__DOCA
##     FC = gaudimodule.gbl.Hlt.FC

# enums of the L0 calo
LODU_CALOELE   = 0
L0DU_CALOGAMMA = 1
LODU_CALOHAD   = 2
LODU_CALOPI0   = 3
LODU_CALOPI0GLOBAL   = 4
LODU_CALOSUMET   = 5
L0DU_CALOPSDMULT = 6

# enums of the L0DUChannel 
L0DUCHANNEL_ELE = 5
L0DUCHANNEL_GAM = 7
L0DUCHANNEL_HAD = 9
L0DUCHANNEL_P0L = 11
L0DUCHANNEL_P0G = 13
L0DUCHANNEL_MU1 = 15
L0DUCHANNEL_MU2 = 17

# enums of tracks
TRACKTYPE_LONG = 3
TRACKFLAG_BACKWARD = 1
TRACKINFO_IP = 10

# some global parameters
Z_MAGNET = 5250. #mm
K_MAGNET = 1263. #MeV

# some global cuts
CUT_ET = 3500. # MeV
CUT_CHI2 = 5.
CUT_IPMIN = 0.
CUT_IPMAX = 1e6

# a list of the steps of the trigger
RATES = ["Entries","NMC",
         "L0","L0Calo","L0Conf:NCalos"]
COUNTER = {}
for key in RATES: COUNTER[key] = 0
AVES = {}
for key in RATES: AVES[key] = 0


# a dictionary with histogram and tools
HISTOS = {}
TOOLS  = {}

#------------------- initialize Run -------------------------

def book():
    """ book histograms
    """

    if (not DO_HISTOS): return

    def hbook(key,n,x0,xf,title=""):
        """ book a 1D histogram
        """
        if (title == ""): title = key
        HISTOS[key] = TH1F(key,title,n,x0,xf)

    def hbook2(key,nx,x0,xf,ny,y0,yf,title=""):
        """ book a 2D histogram
        """ 
        if (title == ""): title = key
        HISTOS[key] = TH2F(key,title,nx,x0,xf,ny,y0,yf)

    # book histogram for the steps of the HLT
    for key in RATES:
        hbook(key,20,0,20)

    # some extra histograms
    hbook("MC:Pt",100,0,6000)
    hbook("MC:Pt1",100,0,6000)

def hfill(name,x,w=1.):
    "fill a 1D histogram"
    if (not DO_HISTOS): return
    HISTOS[name].Fill(x,w)

def tools():
    """ create the tools and locate them into the TOOLS dictionary
    """
    tool1 = gaudi.toolsvc().create("PrepareMuonSeed",interface="IPrepareMuonSeed")
    TOOLS["PrepareMuonSeed"] = tool1

    tool2 = gaudi.toolsvc().create("PrepareHadronSeed",interface="IPrepareCaloSeed")
    TOOLS["PrepareHadronSeed"] = tool2

    tool3 = gaudi.toolsvc().create("PrepareElectronSeed",interface="IPrepareCaloSeed")
    TOOLS["PrepareElectronSeed"] = tool3

    toolA = gaudi.toolsvc().create("TsaConfirmTool",interface="ITrackConfirmTool")
    TOOLS["TsaConfirmTool"] = toolA

    tool4 = gaudi.toolsvc().create("HltVeloRZL0HadCaloMatchTool",interface="ITrackL0CaloMatchTool")
    TOOLS["RZVeloL0Conf"] = tool4

    tool5 = gaudi.toolsvc().create("HltVeloL0HadCaloMatchTool",interface="ITrackL0CaloMatchTool")
    TOOLS["VeloL0Conf"] = tool5

    tool6 = gaudi.toolsvc().create("HltDataStore",interface="IHltDataStore")
    TOOLS["HltDataStore"] = tool6

    tool7 = gaudi.toolsvc().create("MatchTVeloTracks",interface="IMatchTVeloTracks")
    TOOLS["MatchTVeloTracks"] = tool7

book()
tools()

#------------------- execute Run -------------------------

def sort_pt(t1,t2):
    """ sort tracks by pt
    """
    if (t1.pt() > t2.pt()): return -1
    elif (t1.pt() < t2.pt()): return +1
    return 0

def l0calos(type = 2, etcut = CUT_ET):
    """ return l0 calo candidates above a given cut in energy
    """
    hcalos = []
    calos = TES["Trig/L0/FullCalo"]
    if (not calos): return hcalos
    for calo in calos:
        if ((calo.type() == type) and (calo.et() >= etcut)):
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
    """ MC: return the tracks linked with MC mcpars
    """
    keys = map(lambda x: x.key(),mcpars)
    def isSignal(track):
        mcpar = link.first(track)
        if (not mcpar): return False
        return (mcpar.key() in keys)
    stracks = filter(isSignal,tracks)
    return stracks

def call0():
    """ select l0-yes and l0-hadron-yes and update counters
    """
    
    l0 = TES["Trig/L0/L0DU"]
    if (not l0): return False
    if (DEBUG): print " l0 ",l0.decision()
    if (not l0.decision()): return False
    COUNTER["L0"] += 1    
    
    chans = [L0DUCHANNEL_ELE,L0DUCHANNEL_GAM]
    ok = False
    for chan in chans:
        ok = ok or l0.channelDecision(chan)
        if (DEBUG): print " l0 had ",l0.channelDecision(chan)
    if (not ok): return False
    COUNTER["L0Calo"] += 1    

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
    """ return the list of tracks1 that were originated from tracks0
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
    """ returns tracks in tracks0 that were ancestors of track1  
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
    Assumes IP is located in the tracks as info!!
    """
    tip = track.info(TRACKINFO_IP,ipmax)
    if (doabs): tip = abs(tip)
    if (VVERBOSE): print tip
    return ( (tip>=ipmin) and (tip < ipmax) )

def filter_pt(track,ptmin = 0.):
    """ filter functio to cut in pt
    """
    return (track.pt() >= ptmin)    

def decision(comment,n,hname, nmin = 1):
    """ method to pass a decision, fill histogram, update step counter etc
    """
    if (DEBUG): print comment,n
    if (n<nmin): return False
    hfill(hname,1.*n,1.)
    COUNTER[hname] +=1
    AVES[hname] += n
    return True
    
def conf_algo():

    COUNTER["Entries"] += 1
    if (DEBUG): print " ************* "

    ok = call0()
    if (not ok): return False

    ecalos = l0calos()
    n = len(ecalos)
    ok = decision(" had calos ",n,"L0Conf:NCalos")
    if (not ok): return False

    return True

def algo_l0conf(calos):

    prepare = TOOLS["PrepareHadronSeed"]
    tsa     = TOOLS["TsaConfirmTool"]
    tvmatch = TOOLS["MatchTVeloTracks"] 

    vtracks = TES["Rec/Track/RZVelo"]
    if (DEBUG): print " rzvelo tracks ",vtracks.size()

    trackspairs = []

    for calo in calos:
        statep = State()
        staten = State()
        states = []
        sc = prepare.prepareSeed(calo,statep,staten)
        if (DEBUG): print " prepare seed et ",calo.et()," pt+ ",statep.pt()," pt- ",staten.pt()
        if (sc.isSuccess()): states = [statep,staten]
        for state in states:
            if (not state): continue
            tracks = VectorTracks()
            sc = tsa.tracks(state,tracks)
            if (DEBUG): print " state pt ",state.pt()," n t-tracks ",tracks.size()
            if (sc.isFailure()): continue
            for track in tracks:
                vseltracks = []
                for vtrack in tracks:
                    sc = tvmatch.match2dVelo(vtrack,track)
                    if (sc.isSuccess()): trackpairs.append( [vtrack,track] )
                if (DEBUG): print " rzvelo-t matched tracks: state pt ",state.pt(), " track pt ",track.pt()
                if (DEBUG): print "          number of matchs ",len(trackspairs)
    if (DEBUG): print " total track pairs ",len(trackspairs)
    return trackspairs
    

def test_alessiastool():
    ftracks = TES["Hlt/Track/Forward"]
    vtracks = TES["Rec/Track/RZVelo"]
    match = TOOLS["MatchTVeloTracks"]
    scs = map(lambda vt: match.match2dVelo(vt,ftracks[0]),vtracks)
    scs = filter(lambda sc: sc.isSuccess(),scs)
    print "good matched ",len(scs)
    
... 
Traceback (most recent call last):
  File "<stdin>", line 1, in ?
AttributeError: 'IMatchTVeloTracks' object has no attribute 'match'
>>> for vt in vtracks: match.match2dVelo(vt,ftracks[0])
... 

class L0Conf(gaudimodule.PyAlgorithm):
    def execute ( self ) :
        conf_algo()
        return True    


print "------------ ready to run ------------ "
alg = L0Conf("VeloL0Conf")
gaudi.addAlgorithm (alg )
gaudi.run(NEVENTS)

#---------- finalize Run -----------------------------

def rates(keys = RATES, norma = 1.):
    """ function to write the rates and cnandidates of the HLT steps
    """
    for key in keys:
        ave = None
        if (AVES.has_key(key) and COUNTER[key]>0):
            ave = float(AVES[key])/float(COUNTER[key])
        print key,norma*COUNTER[key],ave
    

rates()
