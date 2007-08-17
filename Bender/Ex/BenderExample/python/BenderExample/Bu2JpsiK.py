#!/usr/bin/env python2.4
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

from Bender.theMain import *

DECAY  = "[B+ -> J/psi(1S) K+]cc"
ONLY_MC = 0  ### ONLY combinations from DECAY will be computed if option ONLY_MC = 1
SELECT_JPSI = 1
SELECT_KAONS = 1
SELECT_Bu = 1

DST = 0
TUP = 1

#### JPSI CUTS
IDmu = -20.
PTmu = 1000.
JPsiChi2 = 9.
JPsiMASSWINDOW = 50.

#### KAON CUTS
IDk = -20.
IDkp = -20.
PTk = 2000.

####Bu CUTS
BuChi2 = 20.
BuMASSWINDOW = 500.

####Initializing COUNTERs
COUNTER = {}
COUNTER["reco"] = 0
#COUNTER["selected"] = 0
#COUNTER["EventsSelected"] = 0
COUNTER["EVT"] = 0
COUNTER["NoJPsis"] = 0
COUNTER["NoK"] = 0
COUNTER["BenderSel"] = 0

###Defining some variables
GBL = gaudimodule.gbl.LHCb
MJPsi = 3096.920
MBu = 5279.000
AFS = "/afs/cern.ch/user/d/diegoms/DC06/"

def particleSelection(evnt): ## Selecting Muons and Kaons
    
    if ONLY_MC:
        matcher = evnt.mcTruth()
        finder  = evnt.mcFinder()
        Bdec = finder.find( DECAY )
        Bu_MC = Bdec[0]
        bu_id = Bu_MC.particleID().pid()
   
        mcOK = FROMMCTREE(Bu_MC)
        mupMC = evnt.mcselect ("MCmu+", (mcOK) & (MCID == -13))
        mumMC = evnt.mcselect ("MCmu-", (mcOK) & (MCID ==  13))
        
        kMC = evnt.mcselect ("MCk", (mcOK) & (MCABSID ==  321))
      
        
        mupcuts = (ID ==  -13) & (MCTRUTH(matcher, mupMC))
        mumcuts = (ID ==   13) & (MCTRUTH(matcher, mumMC))
        kcuts  = (ABSID ==  321) & (MCTRUTH(matcher, kMC ))
        
    else:
        mupcuts = (ID ==  -13) #& (MCTRUTH(matcher, mupMC))
        mumcuts = (ID ==   13) #& (MCTRUTH(matcher, mumMC))
        kcuts  = (ABSID == 321) #& (MCTRUTH(matcher, kpMC ))
       
    muplus  = evnt.select ( 'mu+' , mupcuts)
    muminus = evnt.select ( 'mu-' , mumcuts)

    kaons  = evnt.select ( 'k' ,kcuts)
   
   
    return muplus, muminus, kaons

def JPsiSelection(evtn):
    candidates = evtn.loop ( "mu+ mu-" , "J/psi(1S)" )
    for candidate in candidates:
        if SELECT_JPSI:
            mu1 = candidate(1)
            mu2 = candidate(2)
            if min(PIDmu(mu1),PIDmu(mu2)) < IDmu: continue
            if min(PT(mu1),PT(mu2)) < PTmu: continue
            if VCHI2(candidate) > JPsiChi2: continue
            if abs(M(candidate) - MJPsi) > JPsiMASSWINDOW: continue
        candidate.save("jpsi")
    return evtn.selected("jpsi")
        

class Bu2JpsiK(AlgoMC):
    #def initialize(self): return True

    def analyse(self):
        COUNTER["EVT"] += 1
        muplus, muminus, kaons = particleSelection(self)
        
        print muplus.size(), muminus.size(), kaons.size()
        rec = int(muplus.size() > 0 and muminus.size() > 0 and kaons.size() > 0 )
        COUNTER["reco"] += rec

        
        jpsis = JPsiSelection(self)
       
        buloop = self.loop("jpsi k", "B+")
        for b in buloop:
        
            jpsi, kaon = b(1), b(2) # Bu Daughters 
            mu1, mu2 = jpsi.daughters().at(0), jpsi.daughters().at(1) # muons
            
            if SELECT_Bu:
                if PT(kaon) < PTk: continue
                if VCHI2(b) > BuChi2: continue
                if abs(M(b) - MBu ) > BuMASSWINDOW: continue
            b.save("B+")
            if TUP:
                tup = self.nTuple( 'T' )
                tup.column("evt", COUNTER["EVT"])
                #tup.column("sel", w)
                tup.column("JPsiChi2", jpsi.endVertex().chi2())
               
                tup.column("Vchi2", VCHI2(b))
              
                tup.column("JPsiMass", M(jpsi))
                tup.column("Bmass", M(b))
                
                tup.column("mu1p1", PX(mu1))
                tup.column("mu1p2", PY(mu1))
                tup.column("mu1p3", PZ(mu1))
               
                tup.column("mu2p1", PX(mu2))
                tup.column("mu2p2", PY(mu2))
                tup.column("mu2p3", PZ(mu2))
                
                tup.column("k1p1", PX(kaon))
                tup.column("k1p2", PY(kaon))
                tup.column("k1p3", PZ(kaon))
               
                tup.column("QK", Q(kaon))
               
                tup.column("PIDmu1", PIDmu(mu1))
                tup.column("PIDmu2", PIDmu(mu2))
                tup.column("PIDk", PIDK(kaon))
                tup.column("PIDkp",PIDK(kaon) - PIDp(kaon))

                tup.column("SV1", VX(b))
                tup.column("SV2", VY(b))
                tup.column("SV3", VZ(b))
                
                pvs = self.vselect("pvs", ISPRIMARY)
                PVips = VIPCHI2( b.particle(), self.geo())
                PVcut = (PVips >= 0. )
                PV = selectVertexMin(pvs, PVips, PVcut)
                
                tup.column("PV1", VX(PV))
                tup.column("PV2", VY(PV))
                tup.column("PV3", VZ(PV))
                tup.column("Bips" , PVips(PV))
                tup.write()    

        if self.selected("B+").empty():
            return SUCCESS
        self.setFilterPassed( True )
        COUNTER["BenderSel"] += 1
        
       
        return SUCCESS


##SCRIPT CONFIGURATION:
def configure():
    import os 
    import data_Bs2Jpsiphi_mm as input 
    JOBOPTS = os.environ['BENDEREXAMPLEOPTS']+"/DVPreselBu2JPsiK_DC06.opts"
    gaudi.config(
        files   = [ JOBOPTS ],
        options = [ "PoolDbCacheSvc.Catalog = %s "%input.catalog_CERN ]
        )
    
    alg = Bu2JpsiK('Bu2JPsiK')
    
    gaudi.addAlgorithm(alg)
    gaudi.ntupleSvc().Output = [ "FILE1 DATAFILE='Bu2JpsiK.root' OPT='NEW' TYP='ROOT'" ]
    
    gaudi.evtSel().open( input.FILEs )
    alg.NTupleLUN = 'T'

    desktop = gaudi.tool('Bu2JPsiK.PhysDesktop')
    desktop.InputLocations = ['/Event/Phys/StdLooseKaons' ,'/Event/Phys/StdLooseMuons' ] #Loading Pions and Kaons

# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print __doc__

    configure()
    N = 100
    gaudi.run(N)
    gaudi.finalize()
    
    print COUNTER

# =============================================================================
# The END 
# =============================================================================
