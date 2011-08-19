import os
import loadModule

DEBUG=False
DATA="2008"
VERSION="def"

FILENAME = "M_"+DATA+"_"+VERSION
FILENAMEDEF0 = "M_"+DATA+"_def"
FILENAMEDEF1 = "M_DC06_def"


if DEBUG:
    print "# LOADING FROM"
    print "# DATA=",DATA
    print "# VERSION=",VERSION

###############################################################
####################### OFFICIAL MUONID #######################

## Control Flags
# Fill container with all PS Muon Tracks
AllMuonTracks = False   
## Calculate chi2 or set it default to 0
FindQuality = True

## Override data from conditions database, keeping the values from this option file
OverrideDB = False;

## Set which DLL is input to MuonPID
DLL_flag = 1  #0=old; 1=DLL new; 3=hyperbolic tangent mapping

PreSelMomentum = 3000.

##IsMuon parameters
## MuonID momentum cut ranges (MeV/c)
MomentumCuts = [ 6000. , 10000. ]

## MuonID FOI parameters

FOIfactor = 1.2

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
distMuon  = [0.311, 1.349, 0.524, 0.0020, 17., 10.6, 0.04, 4.1, 1.64]
distPion  = [11., -12., 0.2029, -0.026, 0.06, 0.59, 0.008, -29., 41.]

######################################################
####################### FITTER #######################

UseSeedStateErrors = True
FitUpstream = False
MaxNumberOutliers = 0
NumberFitIterations = 1
AddDefaultReferenceNodes = False
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


try: chi2Signal=loadModule.loadFromModule(sname)
except:
    if DEBUG: print "# WARNING: chi2 signal not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    try: chi2Signal=loadModule.loadFromModule(FILENAMEDEF0+"_chi2s")
    except: chi2Signal=loadModule.loadFromModule(FILENAMEDEF1+"_chi2s")

try: chi2Bkg=loadModule.loadFromModule(bname)
except:
    if DEBUG: print "# WARNING: chi2 bkg not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    try: chi2Bkg=loadModule.loadFromModule(FILENAMEDEF0+"_chi2b")
    except: chi2Bkg=loadModule.loadFromModule(FILENAMEDEF1+"_chi2b")





########################################################
####################### CLDIST #########################
sname=FILENAME+"_dists"
bname=FILENAME+"_distb"


try: distSignal=loadModule.loadFromModule(sname)
except:
    if DEBUG: print "# WARNING: dist signal not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    try: distSignal=loadModule.loadFromModule(FILENAMEDEF0+"_dists")
    except: distSignal=loadModule.loadFromModule(FILENAMEDEF1+"_dists")



try: distBkg=loadModule.loadFromModule(bname)
except:
    if DEBUG: print "# WARNING: dist bkg not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    try: distBkg=loadModule.loadFromModule(FILENAMEDEF0+"_distb")
    except: distBkg=loadModule.loadFromModule(FILENAMEDEF1+"_distb")



##########################################################
####################### GETARRIVAL #######################
mname=FILENAME+"_gamoms"
pname=FILENAME+"_gaprobs"


try: gamoms=loadModule.loadFromModule(mname)
except:
    if DEBUG: print "# WARNING: get arrival moms not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    try: gamoms=loadModule.loadFromModule(FILENAMEDEF0+"_gamoms")
    except: gamoms=loadModule.loadFromModule(FILENAMEDEF1+"_gamoms")
    
    
try: gaprobs=loadModule.loadFromModule(pname)
except:
    if DEBUG: print "# WARNING: get arrival probs not available for DATA=%s,VERSION=%s. Loading default" %(DATA,VERSION)
    try: gaprobs=loadModule.loadFromModule(FILENAMEDEF0+"_gaprobs")
    except: gaprobs=loadModule.loadFromModule(FILENAMEDEF1+"_gaprobs")
    



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

# Load parameters for DLL_flag=3: mapping of dist2 with hyperbolic tangent
# Tuning for DC06 data.

from MuonID import tanh_DC06
from tanh_DC06 import *

