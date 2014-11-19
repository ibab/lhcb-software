#################################################################################
## This option file helps you save time by running in sequence                 ##
## RanLengthAna.py and RadLengthAna_VELO.py and merging the outputs.           ##
## You can run this simply by "python Run_RadLenScan.py"                       ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2014-02-14                                      ##
#################################################################################

import sys
import os
import shutil
import fileinput
import string
from RadLengthMakePlots import makePlots

useganga = False


pwd = os.environ['PWD']
outputpath = pwd
if(len(sys.argv) == 2):
    out = sys.argv[1]
else :
    out = 'Rad_merged.root'

os.system("mkdir -p plots")
run_command = "gaudirun.py MaterialEvalGun.py Gauss-Job.py RadLengthAna.py"
os.system(run_command)
run_command = "gaudirun.py MaterialEvalGun.py Gauss-Job.py RadLengthAna_VELO.py"    
os.system(run_command)
    
output=outputpath+'/'+out
merge_command = ' hadd -f ' + output + ' ' + pwd + '/Rad.root ' + pwd + '/Rad_VELO.root '
os.system(merge_command)

makePlots(out,"plots/","rad")
makePlots(out,"plots/","inter")

    
