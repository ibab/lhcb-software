#################################################################################
## This option file helps you save time by running in sequence                 ##
## RanLengthAna.py and RadLengthAna_VELO.py and merging the outputs.           ##
## You can run this simply by "python Run_RadLenScan.py"                       ##
##                                                                             ##
##  @author : L.Pescatore                                                      ##
##  @date   : last modified on 2014-02-14                                      ##
#################################################################################




import sys
import os
import shutil
import fileinput
import string
import subprocess


useganga = False


pwd = os.environ['PWD']
outputpath = pwd
if(len(sys.argv) == 2):
    out = sys.argv[1]
else :
    out = 'Rad_merged.root'

run_command = "gaudirun.py RadLengthAna.py"
os.system(run_command)
run_command = "gaudirun.py RadLengthAna_VELO.py"    
os.system(run_command)
    
output=outputpath+'/'+out
merge_command = ' hadd -f ' + output + ' ' + pwd + '/Rad.root ' + pwd + '/Rad_VELO.root '
os.system(merge_command)
   
    
