#! /usr/bin/env python
import sys, os
from Bender.MainMC import *
from Gaudi.Configuration import NTupleSvc

### Some variables
## AFS = os.environ["HOME"] + "/cmtuser/"

bsmumuCounterKeys = ['CandidatesAnalyzed', 'k_Dz', 'k_muIps', 'k_Ptmu', 'MuMuPhotBsel', 'k_BIps', 'MuMuPhot', 'k_MassWindow', 'k_Chi2', 'k_Bpt', 'EVT', 'k_DOFS']

################################### Some General Options. Remember to check it


TUP = 1

 
###########################
### FOR BATCH MODE ONLY ###
###########################
        ## Use: bsub -q ... Bex_BSMUMU_SCRIPT.py n
        # for running on splited datacards ( ex: bsmumu_n.opts)


datacards = os.environ['BENDEREXAMPLEROOT'] + "/options/bsmumuCERN.opts"
# datacards = os.environ['BENDEREXAMPLEROOT'] + "/options/BuJPsiKCERN.opts"
if not os.path.exists ( datacards ) :
    raise RuntimeError, "DATACARDS-file '%s' does not exist" % datacards



########################
### ALGORITHMS TO ADD ##
########################

__BHH = 1
__BsMuMu = 1
__BuJPsiK = 1

############
### CUTS ###
############

mupcuts = (ID == -13)
mumcuts = (ID ==  13)
MASSWINDOW = 600.
CCh_MASSWINDOW = 500.
Bips_Cut = 6.              ## Of B Candidate
Chi2_Cut = 14.
mIps_Cut = 3.5         ## Muon/Kaon
Bpt_Cut = 700.
signedDOFS_Cut = 12.
JPsiMASSWINDOW = 60.
PhiMASSWINDOW = 28.
PTmu_Cut = 500.   # Max (m+ mu-), required by Heavy DiMuon

if TUP:
    NTupleSvc( Output =["T DATAFILE='BSMUMU_tuples.root' TYP='ROOT' OPT='NEW'"] )

from Bex_BQQ import *  
from Bex_BuJPsiK import *

def configure():
    algos = []
    
    importOptions("$CCBARROOT/options/DVPreselHeavyDimuon.opts")
    importOptions("$COMMONPARTICLESROOT/options/StandardKaons.opts")
    importOptions("$B2UDSROOT/options/DVPreselB2HH.opts")
   
    importOptions(datacards)
    gaudi = appMgr()

    if __BsMuMu:
        alg = B2QQ('Bs2MuMu_Run')
        alg.mupcuts = mupcuts
        alg.mumcuts = mumcuts
        alg.decayname = "B_s0 -> mu+ mu-"
        alg.MASSWINDOW = MASSWINDOW
        alg.ReqOF_Sel = 1
        alg.TUP = TUP
        alg.NTupleLUN = 'T'
        alg.COUNTER = {}
        alg.COUNTER["Bender(evts) " + alg.decayname] = 0
        for key in bsmumuCounterKeys:
            alg.COUNTER[key] = 0.
        algos.append(alg)
        gaudi.addAlgorithm(alg)
        desktop = gaudi.tool('Bs2MuMu_Run.PhysDesktop')
        desktop.InputLocations = ['/Event/Phys/StdLooseMuons' ]
    
    if __BuJPsiK:
        bu = BuJPsiK("BuJPsiK")
        bu.NTupleLUN = "T"
        bu.MASSWINDOW = CCh_MASSWINDOW
        bu.COUNTER = {}
        bu.COUNTER["EVT"] = 0
        bu.COUNTER["Sel"] = 0
        algos.append(bu)
        gaudi.addAlgorithm(bu)
        desktop2 = gaudi.tool('BuJPsiK.PhysDesktop')
        desktop2.InputLocations = ['/Event/Phys/StdUnbiasedHeavyDimuon','/Event/Phys/StdNoPIDsKaons' ]
    
    if __BHH:
        bhh = B2QQ("BenderBHH")
        bhh.mupcuts = (ID == 321)
        bhh.mumcuts = (ID == -321)
        bhh.decayname = "B --> h+ h-"
        bhh.ReqOF_Sel = 1   ### Asks for the official preselection
        bhh.MASSWINDOW = MASSWINDOW
        bhh.TUP = TUP
        bhh.NTupleLUN = "T"
        algos.append(bhh)
        bhh.COUNTER = {}
        bhh.COUNTER["Bender(evts) " + bhh.decayname] = 0
        for key in bsmumuCounterKeys:
            bhh.COUNTER[key] = 0
        gaudi.addAlgorithm(bhh)
        desktop4 = gaudi.tool('BenderBHH.PhysDesktop')
        desktop4.InputLocations = ['/Event/Phys/StdNoPIDsKaons']


    ########################
    ##COMMON ATRIBUTES ####
    ######################
    
    for algo in algos:
        algo.Bips_Cut = Bips_Cut             
        algo.Chi2_Cut = Chi2_Cut
        algo.mIps_Cut = mIps_Cut         ## Muon
        algo.Bpt_Cut = Bpt_Cut
        algo.PTmu_Cut = PTmu_Cut
        algo.signedDOFS_Cut = signedDOFS_Cut
        

configure()
N = 1000
run(N)
appMgr().stop()
appMgr().finalize()
