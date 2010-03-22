import os

DEBUG=False
DATA="2008"
VERSION="def"

MYROOT = os.environ["MUONPIDCHECKERROOT"]+"/python/MuonPIDChecker/"
FILENAME = MYROOT+DATA+"_"+VERSION
FILENAMEDEF0 = MYROOT+DATA+"_def"
FILENAMEDEF1 = MYROOT+"DC06_def"

if DEBUG:
    print "LOADING FROM"
    print "DATA=",DATA
    print "VERSION=",VERSION

#-------------------------------------------------------------
#    IsMuonLoose, IsMuon, MuProb, MuProb, DLL, DLL, NShared, NShared
MonitorCutValues = [1., 1., 0.9, 0.9, 1.4, 1.4, 1., 1. ]
