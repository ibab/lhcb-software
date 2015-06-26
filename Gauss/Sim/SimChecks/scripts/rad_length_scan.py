#!/usr/bin/env python
#################################################################################
## This option file helps you save time by running in sequence                 ##
## RanLengthAna.py and RadLengthAna_VELO.py and merging the outputs.           ##
## You can run this simply by "python rad_length_scan.py"                      ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2015-06-16                                      ##
#################################################################################

import sys
import os

base = os.environ["SIMCHECKSROOT"] + "/options/RadLength/"
from RadLengthMakePlots import makePlots

useganga = False

pwd = os.environ['PWD']
outputpath = pwd
out = 'Rad_merged.root'
if(len(sys.argv) == 2):
    out = sys.argv[1]

os.system("mkdir -p plots")
cmd = "gaudirun.py {base}/MaterialEvalGun.py {base}/Gauss-Job.py {base}".format(base=base)
os.system(cmd+"RadLengthAna.py")
os.system(cmd+"RadLengthAna_VELO.py")
    
output=outputpath+'/'+out
merge_command = 'hadd -f {output} {pwd}/Rad.root {pwd}/Rad_VELO.root'.format(output=output, pwd=pwd)
os.system(merge_command)

makePlots(out,"plots/","rad")
makePlots(out,"plots/","inter")

    
