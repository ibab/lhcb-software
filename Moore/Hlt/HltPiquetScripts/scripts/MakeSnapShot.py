# MakeSnapShot.py
# Author: evh, 18/02/2009
# Looks for latest version of SQLDDDB
# Parses conddb/release_notes.xml and makes a snapshot for all tags in the
# latest release
# to be called as a post install procedure for SQLDDDB

from CondDBUI.Admin.TagsFilter import last_gt_lts,all_gts

import sys
import os

#DataType = "HLT"
DataType = "2015"
		    

def main(): 
   all_gts("LHCBCOND",DataType)
   last_gt_lts("LHCBCOND",DataType)
   if last_gt_lts("LHCBCOND",DataType) is None:
      print "No LHCBCOND tag."
   else:  
      print "LHCBCOND" 
      print last_gt_lts("LHCBCOND",DataType)[0]
      answer=raw_input("Make snapshot? y or n")
      if answer == "y":
         os.system('./MakeSnapShot.sh '+last_gt_lts("LHCBCOND",DataType)[0])
      else:
         print "No snapshot made."	 
   all_gts("DDDB",DataType)
   last_gt_lts("DDDB",DataType)
   if last_gt_lts("DDDB",DataType)[0] is None:
      print "No DDDB tag."
   else:   
      print "DDDB"
      print last_gt_lts("DDDB",DataType)[0]
      answer=raw_input("Make snapshot? y or n")
      if answer == "y":
         os.system('./MakeSnapShot.sh '+last_gt_lts("DDDB",DataType)[0])
      else:
         print "No snapshot made."	
   all_gts("SIMCOND",DataType)
   last_gt_lts("SIMCOND",DataType)
   if last_gt_lts("SIMCOND",DataType) is None:
      print "No SIMCOND tag."
   else:   
      print "SIMCOND"
      print last_gt_lts("SIMCOND",DataType)[0]   
      answer=raw_input("Make snapshot? y or n")
      if answer == "y":
         os.system('./MakeSnapShot.sh '+last_gt_lts("SIMCOND",DataType)[0])
      else:
         print "No snapshot made."	
#   

    
if __name__ == '__main__':
   main()    
