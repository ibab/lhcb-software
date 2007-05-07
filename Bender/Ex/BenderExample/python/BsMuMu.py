#!/usr/bin/env python
# =============================================================================
# $Id: BsMuMu.py,v 1.1 2007-05-07 13:58:09 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
# =============================================================================
## The simple Bender-based example from Diego Martinez Santos
#
#  @date 2007-05-07
#  @author Diego Martinez Santos Diego.Martinez.Santos@cern.ch
#
# =============================================================================
""" The simple Bender-based example from Diego Martinez Santos """
# =============================================================================

import sys

from bendermodule import *
from math import sqrt as s_q_r_t

################################### Some General Options


DECAY  = "[B_s0 -> mu+ mu-]cc"
DECAY_DESCRIPTORS = ["B_s0 -> mu+ mu-","B_s0 -> J/psi(1S) phi(1020)"," [B+ -> J/psi(1S) K+]cc " ]

SELECTION = 1
CONTROL_CHANNELS = 1
CCh_REQUIRE_HEAVY_DIMUON = 1  ### ONLY FOR CONTROL CHANNELS

TUP = 1
TRIGGER = 1
#### BSMUMU CUTS
angle, Chi2, IPChi2, MASSWINDOW = 0.1, 14., 6.* 6 , 600.

#### JPSI CUTS (for B+ --> JPsi K+ )
PTmu_1, JPsiChi2_1 , JPsiMASSWINDOW_1 = 1000., 9., 50.

#### KAON CUTS (for B+ --> JPsi K+ )
PTk_1 = 2000.

####Bu CUTS
BuChi2, BuMASSWINDOW = 20., 500.

#### JPSI CUTS (for Bs --> JPsi Phi)
IDmu_2, PTmu_2, JPsiChi2_2, JPsiMASSWINDOW_2 = -1., 1500., 6., 85.

#### PHI CUTS
IDk_2, PTk_2, PhiChi2, PhiMASSWINDOW = 0., 750., 40., 28.

####BS CUTS (for Bs --> JPsi Phi)
BsChi2, BsMASSWINDOW = 22.5, 100.

####Initializing COUNTERs
COUNTER = {}
COUNTER["MuMuCouplesAnalyzed"] = 0
COUNTER["EVT"] = 0
COUNTER["MuMuPhot"]= 0
COUNTER["MuMuPhotBsel"]= 0
for channel in DECAY_DESCRIPTORS:
    COUNTER[channel] = 0
    COUNTER["Bender " + channel] = 0


###Defining some variables
GBL = gaudimodule.gbl.LHCb
MJPsi = 3096.920
MPhi = 1019.460
MBs = 5369.600
MBu = 5279.000

def particleSelection(evnt): ## Selecting Muons and Kaons
    mupcuts = (ID ==  -13) 
    mumcuts = (ID ==   13) 
    kcuts  = (ABSID == 321)
    kpcuts = ID == 321
    kmcuts = ID == -321
       
    muplus  = evnt.select ( 'mu+' , mupcuts)
    muminus = evnt.select ( 'mu-' , mumcuts)

    kaons  = evnt.select ( 'k' ,kcuts)
    
    kplus  = evnt.select ( 'k+' , kpcuts)
    kminus = evnt.select ( 'k-' , kmcuts)
   
    return muplus, muminus, kaons, kplus, kminus

def JPsiSelection_Bu2JPsiK(evtn):
    candidates = evtn.loop ( "mu+ mu-" , "J/psi(1S)" )
    for candidate in candidates:
        #if SELECT_JPSI_Bu2JPsiK:
        mu1 = candidate(1)
        mu2 = candidate(2)
        #if min(PIDmu(mu1),PIDmu(mu2)) < IDmu: continue
        if min(PT(mu1),PT(mu2)) < PTmu_1: continue
        if VCHI2(candidate) > JPsiChi2_1: continue
        if abs(M(candidate) - MJPsi) > JPsiMASSWINDOW_1: continue
        candidate.save("jpsi_1")
    return evtn.selected("jpsi_1")

def JPsiSelection_Bs2JPsiPhi(evtn):
    candidates = evtn.loop ( "mu+ mu-" , "J/psi(1S)" )
    for candidate in candidates:
        #if SELECT_JPSI_Bs2JPsiPhi:
        mu1 = candidate(1)
        mu2 = candidate(2)
        if max(PIDmu(mu1),PIDmu(mu2)) < IDmu_2: continue
        if max(PT(mu1),PT(mu2)) < PTmu_2: continue
        if VCHI2(candidate) > JPsiChi2_2: continue
        if abs(M(candidate) - MJPsi) > JPsiMASSWINDOW_2: continue
        candidate.save("jpsi_2")
    return evtn.selected("jpsi_2")
        
def PhiSelection(evtn):
    candidates = evtn.loop("k+ k-", "phi(1020)")
    for candidate in candidates:
        #if SELECT_PHIS:
        k1 = candidate(1)
        k2 = candidate(2)
        if max(PIDK(k1),PIDK(k2)) < IDk_2: continue
        if max(PT(k1),PT(k2)) < PTk_2: continue
        if VCHI2(candidate) > PhiChi2: continue
        if abs(M(candidate) - MPhi) > PhiMASSWINDOW: continue
        candidate.save("phi")
    return evtn.selected("phi")

def BsJPsiPhi(evtn, SelDic, COUNTER, TES):
    ########################################### STARTING  Bs --> JPsi Phi
    if not SelDic["B_s0 -> J/psi(1S) phi(1020)"]: return 0   ### No Pass, no entry !!
    
    jpsis_2 = JPsiSelection_Bs2JPsiPhi(evtn)
    phis = PhiSelection(evtn)
    if TRIGGER: 
        L0 = TES["Trig/L0/L0DU"].decision()
        HLT = TES["Hlt/Summary"].pattern()
   
    for b in evtn.loop("jpsi_2 phi", "B_s0"):
        
        jpsi, phi = b(1), b(2) # Composed Particles
        mu1, mu2 = jpsi.daughters().at(0), jpsi.daughters().at(1) # muons
        k1, k2 = phi.daughters().at(0), phi.daughters().at(1) #Kaons
        
        #if SELECT_BS:
        if VCHI2(b) > BsChi2: continue
        if abs(M(b) - MBs ) > BsMASSWINDOW: continue
        b.save("Bs_jp")
        #if TUP:
        tupBs = evtn.nTuple( 'B_s0 -> J/psi(1S) phi(1020)' )
        tupBs.column("evt", COUNTER["EVT"])                 ### FILLING Bs --> JPsi Phi
        #tupBs.column("sel", w)
        tupBs.column("JPsiChi2", jpsi.endVertex().chi2())                 ### FILLING Bs --> JPsi Phi
        tupBs.column("PhiChi2", phi.endVertex().chi2())                 ### FILLING Bs --> JPsi Phi
        tupBs.column("Vchi2", VCHI2(b))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("PhiMass", M(phi))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("JPsiMass", M(jpsi))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("Bmass", M(b))                 ### FILLING Bs --> JPsi Phi
        
        tupBs.column("mu1p1", PX(mu1))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("mu1p2", PY(mu1))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("mu1p3", PZ(mu1))                 ### FILLING Bs --> JPsi Phi
        
        tupBs.column("mu2p1", PX(mu2))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("mu2p2", PY(mu2))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("mu2p3", PZ(mu2))                 ### FILLING Bs --> JPsi Phi
        
        tupBs.column("k1p1", PX(k1))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("k1p2", PY(k1))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("k1p3", PZ(k1))                 ### FILLING Bs --> JPsi Phi
        
        tupBs.column("k2p1", PX(k2))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("k2p2", PY(k2))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("k2p3", PZ(k2))                 ### FILLING Bs --> JPsi Phi
        
        tupBs.column("PIDmu1", PIDmu(mu1))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("PIDmu2", PIDmu(mu2))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("PIDk1", PIDK(k1))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("PIDk2",PIDK(k2))                 ### FILLING Bs --> JPsi Phi
        
        tupBs.column("SV1", VX(b))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("SV2", VY(b))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("SV3", VZ(b))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("evtNum", TES["Rec/Header"].evtNumber())                 ### FILLING Bs --> JPsi Phi
        tupBs.column("runNum", TES["Rec/Header"].runNumber())                 ### FILLING Bs --> JPsi Phi

        tupBs.column("Bsmumu", SelDic["B_s0 -> mu+ mu-"])                 ### FILLING Bs --> JPsi Phi
        tupBs.column("BsJPsiPhi", SelDic["B_s0 -> J/psi(1S) phi(1020)"])                 ### FILLING Bs --> JPsi Phi
        tupBs.column("BuJPsiK", SelDic[DECAY_DESCRIPTORS[2]])                 ### FILLING Bs --> JPsi Phi
        
        pvs_ = evtn.vselect("pvs", ISPRIMARY)                 ### FILLING Bs --> JPsi Phi
        PVips = VIPCHI2( b.particle(), evtn.geo())                 ### FILLING Bs --> JPsi Pih
        PVcut = (PVips >= 0. )                 ### FILLING Bs --> JPsi Phi
        PV = selectVertexMin(pvs_, PVips, PVcut)                 ### FILLING Bs --> JPsi Phi
        if TRIGGER:
            tupBs.column("L0", L0)
            tupBs.column("Hlt", HLT)
               
        tupBs.column("PV1", VX(PV))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("PV2", VY(PV))                 ### FILLING Bs --> JPsi Phi
        tupBs.column("PV3", VZ(PV))                 ### FILLING Bs --> JPsi Phi
        #print PVips(PV)
        tupBs.column("Bips" , s_q_r_t(PVips(PV)))                 ### FILLING Bs --> JPsi Phi
       
        
        tupBs.write()                                  ### FILLING Bs --> JPsi Phi
            
    if evtn.selected("Bs_jp").size():
        COUNTER["Bender B_s0 -> J/psi(1S) phi(1020)"] += 1
            #self.setFilterPassed( True )
    return 1


def BuJPsiK(evtn, SelDic, COUNTER, TES):
    if not SelDic[DECAY_DESCRIPTORS[2]]: return 0   ### No Pass, no entry !!

    
    jpsis_1 = JPsiSelection_Bu2JPsiK(evtn)
    if TRIGGER: 
        L0 = TES["Trig/L0/L0DU"].decision()
        HLT = TES["Hlt/Summary"].pattern()
    for b in evtn.loop("jpsi_1 k", "B+"):
        jpsi, kaon = b(1), b(2) # Bu Daughters 
        mu1, mu2 = jpsi.daughters().at(0), jpsi.daughters().at(1) # muons
        
        if PT(kaon) < PTk_1 : continue
        
        if VCHI2(b) > BuChi2: continue
        if abs(M(b) - MBu ) > BuMASSWINDOW: continue
        b.save("B+")
        tup = evtn.nTuple( DECAY_DESCRIPTORS[2] )                 ### FILLING B+ --> JPsi K+
        tup.column("evt", COUNTER["EVT"])                 ### FILLING B+ --> JPsi K+
        tup.column("JPsiChi2", jpsi.endVertex().chi2())                 ### FILLING B+ --> JPsi K+
        
        tup.column("Vchi2", VCHI2(b))                 ### FILLING B+ --> JPsi K+
        
        tup.column("JPsiMass", M(jpsi))                 ### FILLING B+ --> JPsi K+
        tup.column("Bmass", M(b))                 ### FILLING B+ --> JPsi K+
        
        tup.column("mu1p1", PX(mu1))                 ### FILLING B+ --> JPsi K+
        tup.column("mu1p2", PY(mu1))                 ### FILLING B+ --> JPsi K+
        tup.column("mu1p3", PZ(mu1))                 ### FILLING B+ --> JPsi K+
               
        tup.column("mu2p1", PX(mu2))                 ### FILLING B+ --> JPsi K+
        tup.column("mu2p2", PY(mu2))                 ### FILLING B+ --> JPsi K+
        tup.column("mu2p3", PZ(mu2))                 ### FILLING B+ --> JPsi K+
                
        tup.column("k1p1", PX(kaon))                 ### FILLING B+ --> JPsi K+
        tup.column("k1p2", PY(kaon))                 ### FILLING B+ --> JPsi K+
        tup.column("k1p3", PZ(kaon))                 ### FILLING B+ --> JPsi K+
               
        tup.column("QK", Q(kaon))                 ### FILLING B+ --> JPsi K+
        
        tup.column("PIDmu1", PIDmu(mu1))                 ### FILLING B+ --> JPsi K+
        tup.column("PIDmu2", PIDmu(mu2))                 ### FILLING B+ --> JPsi K+
        tup.column("PIDk", PIDK(kaon))                 ### FILLING B+ --> JPsi K+
        tup.column("PIDkp",PIDK(kaon) - PIDp(kaon))                 ### FILLING B+ --> JPsi K+
                
        tup.column("SV1", VX(b))                 ### FILLING B+ --> JPsi K+
        tup.column("SV2", VY(b))                 ### FILLING B+ --> JPsi K+
        tup.column("SV3", VZ(b))                 ### FILLING B+ --> JPsi K+
        
        tup.column("evtNum", TES["Rec/Header"].evtNumber())                 ### FILLING B+ --> JPsi K+
        tup.column("runNum", TES["Rec/Header"].runNumber())                 ### FILLING B+ --> JPsi K+
        tup.column("Bsmumu", SelDic["B_s0 -> mu+ mu-"])                 ### FILLING Bs --> JPsi Phi
        tup.column("BsJPsiPhi", SelDic["B_s0 -> J/psi(1S) phi(1020)"])                 ### FILLING Bs --> JPsi Phi
        tup.column("BuJPsiK", SelDic[DECAY_DESCRIPTORS[2]])                 ### FILLING Bs --> JPsi Phi
        
        pvs_ = evtn.vselect("pvs", ISPRIMARY)                 ### FILLING B+ --> JPsi K+
        PVips = VIPCHI2( b.particle(), evtn.geo())                 ### FILLING B+ --> JPsi K+
        PVcut = (PVips >= 0. )                 ### FILLING B+ --> JPsi K+
        PV = selectVertexMin(pvs_, PVips, PVcut)                 ### FILLING B+ --> JPsi K+
                
        tup.column("PV1", VX(PV))                 ### FILLING B+ --> JPsi K+
        tup.column("PV2", VY(PV))                 ### FILLING B+ --> JPsi K+
        tup.column("PV3", VZ(PV))                 ### FILLING B+ --> JPsi K+
        if TRIGGER:
            tup.column("L0", L0)
            tup.column("Hlt", HLT)
        
        tup.column("Bips" , s_q_r_t(PVips(PV)))                 ### FILLING B+ --> JPsi K+
       
                
        tup.write()                     ### FILLING B+ --> JPsi K+

    if evtn.selected("B+").size():
        COUNTER["Bender " + DECAY_DESCRIPTORS[2]] += 1
    return 1



class Bs2MuMu_Run(AlgoMC):                                       ##### STARTING THE ALGORITHM
    #def initialize(self): return True

    def analyse(self):
        COUNTER["EVT"] += 1
        pvs = self.vselect("pvs", ISPRIMARY)
        if pvs.empty():
            print "( oo ) NO PV's "
            return SUCCESS
        muplus, muminus, kaons, kplus, kminus = particleSelection(self)
       
#        rec = int( muplus.size() > 0 and muminus.size() > 0 )
        
        TES = gaudi.evtsvc()
        
        ###### TRIGGER
        if TRIGGER:
            L0 = TES["Trig/L0/L0DU"].decision()
            HLT = TES["Hlt/Summary"].pattern()
        
        #### Cheking Selection
        selections =  TES["Phys/Selections"].containedObjects()  ## Looking at Offline selections results at TES

        SelDic = {"B_s0 -> mu+ mu-" : 0,"B_s0 -> J/psi(1S) phi(1020)": 0,DECAY_DESCRIPTORS[2]: 0}
        allow = 1
        for selection in selections:
            channel = selection.decay()
            if CCh_REQUIRE_HEAVY_DIMUON and selection.location() == '/Event/Phys/PreselHeavyDimuon' and not selection.found(): allow = 0
            if channel in DECAY_DESCRIPTORS:           ## Making a Table which tell us if some selection was passed
                SelDic[channel] = selection.found()
                #COUNTER[channel] += selection.found()
                if selection.found(): print " -_- ", channel
        
        SelDic[DECAY_DESCRIPTORS[1]] = SelDic[DECAY_DESCRIPTORS[1]] * allow
        SelDic[DECAY_DESCRIPTORS[2]] = SelDic[DECAY_DESCRIPTORS[2]] * allow
        for channel in DECAY_DESCRIPTORS:
            COUNTER[channel] += SelDic[channel]
            
        if CONTROL_CHANNELS:
            BsJPsiPhi(self, SelDic, COUNTER, TES)  ### This function fills Bs --> JPsi Phi TTree if such preselection was passed
            BuJPsiK(self, SelDic, COUNTER, TES)    ### This function fills B+ --> JPsi K + TTree if such preselection was passed
       
        bsloop = self.loop("mu+ mu-", "B_s0")
       
        for b in bsloop:
            mu1, mu2 = b(1), b(2)
           
            COUNTER["MuMuCouplesAnalyzed"] += 1
           
            PVips = VIPCHI2( b.particle(), self.geo() )
            PVcut = (PVips >= 0. )
            pvs = self.vselect("pvs", ISPRIMARY)
            PV = selectVertexMin( pvs, PVips, PVcut )
           
           
            if SELECTION:
                
                if abs(M(b) - MBs ) > MASSWINDOW: continue
                if VCHI2(b) > Chi2: continue
                
                if PVips(PV) > IPChi2: continue
                
            b.save("Bs")
            
            if TUP:
                tup = self.nTuple( 'T' )#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("evt", COUNTER["EVT"])#FILLING BS --> mu+ mu- NTUple !!!
#print dir(mu1.proto())
                longTracks = []
                for track in TES["Rec/Track/Best"].containedObjects():
                    if track.type() == 3: longTracks.append(track)
                tup.column("longTrancks", len(longTracks))

                o1, o2 = mu1.proto().track().position(), mu2.proto().track().position()
                
                tup.column("mu1p1", PX(mu1))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu1p2", PY(mu1))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu1p3", PZ(mu1))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu2p1", PX(mu2))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu2p2", PY(mu2))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu2p3", PZ(mu2))#FILLING BS --> mu+ mu- NTUple !!!

                tup.column("mu1o1", o1.x())#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu1o2", o1.y())#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu1o3", o1.z())#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu2o1", o2.x())#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu2o2", o2.y())#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("mu2o3", o2.z())#FILLING BS --> mu+ mu- NTUple !!!
               
                tup.column("PIDmu1", PIDmu(mu1))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PIDmu2", PIDmu(mu2))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PIDk1", PIDK(mu1))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PIDk2", PIDK(mu2))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PIDe1", PIDe(mu1))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PIDe2", PIDe(mu2))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PIDp1", PIDp(mu1))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PIDp2", PIDp(mu2))#FILLING BS --> mu+ mu- NTUple !!!
                
                tup.column("SV1", VX(b))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("SV2", VY(b))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("SV3", VZ(b))#FILLING BS --> mu+ mu- NTUple !!!
                if TRIGGER:
                    tup.column("L0", L0)
                    tup.column("Hlt", HLT)    
                
                tup.column("PV1", VX(PV))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PV2", VY(PV))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PV3", VZ(PV))#FILLING BS --> mu+ mu- NTUple !!!

                  
                SVco = b.vertex().covMatrix()
                PVco = PV.covMatrix()
                
                tup.column("VcoX", SVco(1,1))
                tup.column("VcoY", SVco(2,2))
                tup.column("VcoZ", SVco(3,3))
                tup.column("PVcoX", PVco(1,1))
                tup.column("PVcoY", PVco(2,2))
                tup.column("PVcoZ", PVco(3,3))

                mulist = []
                
                for mu in muplus:
                    mulist.append(mu)
                for mu in muminus:
                    mulist.append(mu)
                
                tup.column("Bsmumu", SelDic["B_s0 -> mu+ mu-"])                
                tup.column("BsJPsiPhi", SelDic["B_s0 -> J/psi(1S) phi(1020)"])
                tup.column("BuJPsiK", SelDic[DECAY_DESCRIPTORS[2]])    
                tup.column("Bips" , s_q_r_t(PVips(PV)))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("Vchi2", VCHI2(b))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("PVchi2", PV.chi2())
                tup.column("Bmass", M(b))#FILLING BS --> mu+ mu- NTUple !!!
                tup.column("evtNum", TES["Rec/Header"].evtNumber())
                tup.column("runNum", TES["Rec/Header"].runNumber())

                minIps = MIPCHI2(pvs,self.geo())

                tup.column("mu1ips", s_q_r_t(minIps( mu1 )))
                tup.column("mu2ips", s_q_r_t(minIps( mu2 )))
                
                tup.write()


                
        if self.selected("Bs").empty():
            return SUCCESS
        
        
        COUNTER["Bender B_s0 -> mu+ mu-"] += 1
       
        self.setFilterPassed( True )
       
        return SUCCESS


##SCRIPT CONFIGURATION:                                  # -_- # -_-



def configure():
    import os 
    JOBOPTS = os.environ['BENDEREXAMPLEOPTS']+"/DaVinciBs2MuMurun.opts"
    gaudi.config(files = [JOBOPTS,])
    alg = Bs2MuMu_Run('Bs2MuMu_Run')
    
    gaudi.addAlgorithm(alg)
    gaudi.ntupleSvc().Output = [ "FILE1 DATAFILE='BsMuMu.root' OPT='NEW' TYP='ROOT'" ]
    import data_Bs2Jpsiphi_mm
    gaudi.evtSel().open( data_Bs2Jpsiphi_mm.PFNs )
    alg.NTupleLUN = 'T'    
    desktop = gaudi.tool('Bs2MuMu_Run.PhysDesktop')
    desktop.InputLocations = ['/Event/Phys/StdLooseKaons','/Event/Phys/StdLooseMuons' ] #Loading Pions and Kaons

# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print __doc__

    configure()
    N = -1
    gaudi.run(N)
    gaudi.finalize()
    
    print COUNTER

# =============================================================================
# $Log: not supported by cvs2svn $
#
# =============================================================================
# The END 
# =============================================================================
