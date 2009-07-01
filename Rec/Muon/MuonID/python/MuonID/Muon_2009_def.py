import os
import extpick

DEBUG=False
DATA="2009"
VERSION="def"

PYMUONIDROOT = os.environ["MUONIDROOT"]+"/python/MuonID/"
FILENAME = PYMUONIDROOT+DATA+"_"+VERSION
FILENAMEDEF0 = PYMUONIDROOT+DATA+"_def"
FILENAMEDEF1 = PYMUONIDROOT+"DC06_def"

if DEBUG:
    print "LOADING FROM"
    print "DATA=",DATA
    print "VERSION=",VERSION

###############################################################
####################### OFFICIAL MUONID #######################

## Control Flags
# Fill container with all PS Muon Tracks
AllMuonTracks = False   
## Calculate chi2 or set it default to 0
FindQuality = True 
## Set which DLL is input to MuonPID
DLL_flag = 1  #0=old; 1=DLL new;
## Use arrival probability as weight to IsMuon/IsMuonLoose and Dist
Weight_flag = False

PreSelMomentum = 3000.

##IsMuon parameters
## MuonID momentum cut ranges (MeV/c)
MomentumCuts = [ 6000. , 10000. ]

## MuonID FOI parameters

FOIfactor = 1.2

print "MuonID WARNING: FOI tuning not ready for DATA=%s,VERSION=%s. Loading FOIfactor=%f" %(DATA,VERSION,FOIfactor)
XFOIParameter1 = [5.5, 4.0, 3.3, 2.8,
                  5.2, 3.6, 2.4, 2.4,
                  5.7, 4.4, 2.8, 2.3,
                  5.1, 3.1, 2.3, 2.1,
                  5.8, 3.4, 2.6, 2.8]

XFOIParameter2 = [11., 3., 1., 1.,
                  31., 28., 21., 17.,
                  30., 31., 27., 22.,
                  28., 33., 35., 47.,
                  31., 39., 56., 151.]

XFOIParameter3 = [0.2, 0.08, 0.03, 0.1,
                  0.06, 0.08, 0.10, 0.15,
                  0.04, 0.06, 0.09, 0.12,
                  0.08, 0.15, 0.23, 0.36,
                  0.07, 0.14, 0.24, 0.49]

YFOIParameter1 = [2.8, 1.7, -153., 1.9,
                  3.3, 2.1, 1.7, 1.6,
                  3.6, 2.8, 1.9, 1.8,
                  4.4, 3.3, 2.2, 2.2,
                  4.8, 3.9, 2.6, 2.3]

YFOIParameter2 = [3., 2., 156., 0.,
                  17., 15., 9., 5.,
                  26., 25., 16., 15.,
                  30., 49., 57., 92.,
                  32., 55., 96., 166.]

YFOIParameter3 = [0.03, 0.02, 0.00, 0.09,
                  0.13, 0.19, 0.19, 0.24, 
                  0.11, 0.19, 0.21, 0.32,
                  0.10, 0.22, 0.30, 0.52,
                  0.08, 0.20, 0.34, 0.52]


#//-----------------------------------------------------------------
#// Parameters of the Landau functions for old DLL
#//-----------------------------------------------------------------
if DLL_flag == 0 :
    print "MuonID WARNING: old DLL parameters not tuned for DATA=%s,VERSION=%s. Loading values for DC06" %(DATA,VERSION)
else :
    print "MuonID WARNING: new DLL parameters not tuned for DATA=%s,VERSION=%s. Loading values for DC06" %(DATA,VERSION)
distMuon  = [0.311, 1.349, 0.524, 0.0020, 17., 10.6, 0.04, 4.1, 1.64]
distPion  = [11., -12., 0.2029, -0.026, 0.06, 0.59, 0.008, -29., 41.]

######################################################
####################### FITTER #######################

UseSeedStateErrors = True
FitUpstream = False
MaxNumberOutliers = 0
NumberFitIterations = 1
ZPositions = [11900.]
StateAtBeamLine = False


########################################################
####################### MUIDTOOL #######################

DiscrValue=1
NSigmas=3.5
UseBkg=False
MinMomSt3=3500
MinMomSt4=4500


#######################################################
####################### ISMUONC #######################

MomRangeIsMuon = [3000, 6000, 10000]
IsMuonOpt = 1
MinHits = 2

#########################################################
####################### CLQUALITY #######################

Range=[3000,4000,5000,6000,7000,8000,9000,10000,15000,20000,25000,50000,75000,100000,125000,150000,175000,1000000]
RangeNmuons=[3000,4000,5000,6000,7000,8000,9000,10000,15000,20000,25000,50000,1000000]
LeftRight='right'
Overlap=True
lbinCenter=187500
lbinCenterNmuons=74000
applyOvlapLast=False

######################################################
####################### CLCHI2 #######################
sname=FILENAME+"_chi2s"
bname=FILENAME+"_chi2b"

if os.path.exists(sname): chi2Signal=extpick.pickleLoad(sname)
else:
    print "MuonID WARNING: chi2 signal not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    if os.path.exists(FILENAMEDEF0+"_chi2s"): chi2Signal=extpick.pickleLoad(FILENAMEDEF0+"_chi2s")
    else: chi2Signal=extpick.pickleLoad(FILENAMEDEF1+"_chi2s")

if os.path.exists(bname): chi2Bkg=extpick.pickleLoad(bname)
else:
    print "MuonID WARNING: chi2 bkg not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    if os.path.exists(FILENAMEDEF0+"_chi2b"): chi2Bkg=extpick.pickleLoad(FILENAMEDEF0+"_chi2b")
    else: chi2Bkg=extpick.pickleLoad(FILENAMEDEF1+"_chi2b")





########################################################
####################### CLDIST #########################
sname=FILENAME+"_dists"
bname=FILENAME+"_distb"


if os.path.exists(sname): distSignal=extpick.pickleLoad(sname)
else:
    if DEBUG: print "MuonID WARNING: dist signal not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    if os.path.exists(FILENAMEDEF0+"_dists"): distSignal=extpick.pickleLoad(FILENAMEDEF0+"_dists")
    else: distSignal=extpick.pickleLoad(FILENAMEDEF1+"_dists")



if os.path.exists(bname): distBkg=extpick.pickleLoad(bname)
else:
    if DEBUG: print "MuonID WARNING: dist bkg not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    if os.path.exists(FILENAMEDEF0+"_distb"): distBkg=extpick.pickleLoad(FILENAMEDEF0+"_distb")
    else: distBkg=extpick.pickleLoad(FILENAMEDEF1+"_distb")



##########################################################
####################### GETARRIVAL #######################
mname=FILENAME+"_gamoms"
pname=FILENAME+"_gaprobs"


if os.path.exists(mname): gamoms=extpick.pickleLoad(mname)
else:
    if DEBUG: print "MuonID WARNING: get arrival moms not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    gamoms=extpick.pickleLoad(FILENAMEDEF+"_gamoms")


if os.path.exists(pname): gaprobs=extpick.pickleLoad(pname)
else:
    if DEBUG: print "MuonID WARNING: get arrival probs not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    gaprobs=extpick.pickleLoad(FILENAMEDEF+"_gaprobs")


useFunct=False
alpha=[1,0.0363897,-0.002133,0.000806432,0.0009]
beta=[1,2333.89,2494.38,2837.2,4200]

eff=0.99

##already present for IsMuonC
##MinHits=2

#===========================================================
# G. Lanfranchi & S. Furcas - 10/5/09
#===========================================================
step = 0.2
nMax_bin = 4000
#Region1:
MuLandauParameterR1_1 = [14., 6.6, 0.0, 0.001, 0.0, -1]    
MuLandauParameterR1_2 = [1.,10.,0.0, 0.001, 0.0,-1 ]      
MuLandauParameterR1_3 = [3.4, 2.0, 0.0, 0.001, 0.0,-1]    
MuLandauParameterR1_4 = [1.45, 0.61, 5.76, 2.34, 0.16,-1] 
MuLandauParameterR1_5 = [0.96, 0.37, 3.6, 1.33, 0.26, -1] 
MuLandauParameterR1_6 = [1., 0.43, 3.1, 1.12, 0.17, -1]   
MuLandauParameterR1_7 = [0.74, 0.58, 0.0, 0.0001, 0.0 , -1]
#Region2:
MuLandauParameterR2_1 = [4.5, 1.26, 3.48, 15.06, 0.99,-1] 
MuLandauParameterR2_2 = [2.44, 2.42, 0.0, 0.001, 0.0, -1] 
MuLandauParameterR2_3 = [1.07, 0.46, 3.32, 1.58, 0.25, -1]
MuLandauParameterR2_4 = [0.88, 0.43, 2.9, 0.83, 0.12, -1] 
MuLandauParameterR2_5 = [0.56, 0.23, 1.38, 0.35, 0.40, -1] 
#Region3:
MuLandauParameterR3_1 = [1.8, 0.51, 0.0, 0.001, 0.0, -1] 
MuLandauParameterR3_2 = [1.65, 0.73, 6.33, 3.03, 0.36,-1]
MuLandauParameterR3_3 = [1.3, 0.77, 0.0, 0.001, 0.0,-1 ] 
MuLandauParameterR3_4 = [0.97, 0.37, 0.32, 0.12, 0.5, -1]
MuLandauParameterR3_5 = [0.73, 0.22, 0.27, 0.094, 0.5, -1]
#Region4:
MuLandauParameterR4_1 = [1.67, 2.254, 0., 0.8, 0.0001, -1] 
MuLandauParameterR4_2 = [1.00, 0.45, 3.2, 1.49, 0.5, -1]   
MuLandauParameterR4_3 = [1.42, 0.73, 0.89, 0.5, 0.5, -1]   
MuLandauParameterR4_4 = [0.73, 0.32, 2.175, 0.56, 0.18, -1]
MuLandauParameterR4_5 = [0.67, 0.26, 0.0, 0.001, 0.0, -1 ] 
#==================================
NonMuLandauParameterR1 = [36.5, 23.8, -1] # region 1
NonMuLandauParameterR2 = [40.5, 33.3, -1] # region 2
NonMuLandauParameterR3 = [22.0, 30.0, -1] # region 3
NonMuLandauParameterR4 = [10.0, 17.0, -1] # region 4

# Binning:
nMupBinsR1 = 6
nMupBinsR2 = 4
nMupBinsR3 = 4
nMupBinsR4 = 4
MupBinsR1 = [10000., 20000., 30000., 40000., 50000., 60000.]
MupBinsR2 = [10000., 20000., 30000., 40000.]
MupBinsR3 = [ 6000., 10000., 20000., 30000.]
MupBinsR4 = [ 6000.,  8000., 10000., 15000.]
