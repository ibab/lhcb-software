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
RefEff1IM   = [6.30, 98.95, 64.02, 2.70, 10.19, 4.34] 
RefdEff1IM  = [0.03,  0.03,  0.23, 0.01,  0.02, 0.02] 
RefEff1IML  = [10.84,99.50, 68.07, 5.86, 13.62, 7.53] 
RefdEff1IML = [ 0.04, 0.02,  0.11, 0.02,  0.02, 0.02] 
RefEff1DLL  = [3.59, 94.89, 45.57, 1.37, 7.99, 2.55]  
RefdEff1DLL = [0.02,  0.06,  0.24, 0.01, 0.02, 0.01]   
RefEff1NSH  = [3.21, 95.71, 48.32, 1.79, 8.25, 3.04]  
RefdEff1NSH = [0.02,  0.05,  0.24, 0.01, 0.02, 0.01]   

RefEff2IM   = [6.08, 97.77, 66.18, 2.85, 5.76, 4.61]
RefdEff2IM  = [0.01,  0.08,  0.14, 0.01, 0.01, 0.01]
RefEff2IML  = [10.59,98.78, 69.83, 5.92, 9.40, 7.70]
RefdEff2IML = [ 0.02, 0.06,  0.14, 0.01, 0.01, 0.01]
RefEff2DLL  = [3.05, 91.69, 48.08, 1.33, 3.33, 2.63]
RefdEff2DLL = [0.01,  0.14,  0.15, 0.01, 0.01, 0.01] 
RefEff2NSH  = [3.01, 91.06, 49.08, 2.14, 3.79, 3.45]
RefdEff2NSH = [0.01,  0.14,  0.15, 0.01, 0.01, 0.01] 
