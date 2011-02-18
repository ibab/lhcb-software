# MakeSnapShot.py
# Author: evh, 18/02/2009
# Looks for latest version of SQLDDDB
# Parses conddb/release_notes.xml and makes a snapshot for all tags in the
# latest release
# to be called as a post install procedure for SQLDDDB

from CondDBUI.Admin.TagsFilter import last_gt_lts,all_gts

import sys
import os

		    

def main(): 
   all_gts("LHCBCOND","HLT")
   last_gt_lts("LHCBCOND","HLT")
   print    all_gts("LHCBCOND","HLT") 
   print last_gt_lts("LHCBCOND","HLT")
   all_gts("DDDB","HLT")
   last_gt_lts("DDDB","HLT")
   print    all_gts("DDDB","HLT") 
   print last_gt_lts("DDDB","HLT")
   all_gts("SIMCOND","HLT")
   last_gt_lts("SIMCOND","HLT")
   print    all_gts("SIMCOND","HLT") 
   print last_gt_lts("SIMCOND","HLT")   
#   os.system('./MakeSnapShot.sh '+tag+' '+name)

    
if __name__ == '__main__':
   main()    
