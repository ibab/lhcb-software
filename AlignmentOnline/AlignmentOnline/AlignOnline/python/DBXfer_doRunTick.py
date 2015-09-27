import sys
args=sys.argv[:]
import RunOption
import HLT2Params
import sys
import os
def MakeTick(root,runnr):
  pth = root+"/onl/Conditions/Online/LHCb"
  if not os.access(pth, os.F_OK):
    try:
      os.makedirs(pth,0777)
    except:
      print "Cannot create directory "+pth
  else:
    print "Path "+pth+" already exists"
  xmlf = open(pth+"/RunStamp.xml","w")
  xmlf.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
  xmlf.write('<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">\n')
  xmlf.write('<DDDB>\n')
  xmlf.write('<condition name="RunStamp"> <param name="RunNumber" type="int">'
             +str(runnr)
             +'</param> </condition>\n')
  xmlf.write('</DDDB>\n')
  xmlf.close()

import time
print "===================== Start transferring The Run Tick for Run "+str(RunOption.RunNumber)+" ========================"
RunStart = HLT2Params.RunStartTime
RunEnd = HLT2Params.RunStartTime+HLT2Params.RunDuration
RunStartns = RunStart*1000000000
RunEndns = RunEnd*1000000000
DBString = "CondDBOnline(owner)/ONLINE" # use the central Oracle Database...
print "Operating on Database "+DBString
print "Adding the RunTick to the Database"
print "IoV is ["+str(RunStartns)+","+str(RunEndns)+"], ["+time.ctime(RunStart)+","+time.ctime(RunEnd)+"]"
status = 0
import CondDBUI
import CondDBUI.Admin
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=False)
status = CondDBUI.Admin.MakeDBFromFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStartns,
                                   until = RunEndns,
                                   writeDuplicate = False
                                    )
db.closeDatabase()
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=True)
print "Checking the Run Tick"
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStartns,
                                   until = RunEndns,
                                    )
if Fails:
  print "Readback check on DB failed\n"
  pprint(Fails)
  sys.exit(44)
db.closeDatabase()
print "===================== Done transferring The Run Tick for Run "+str(RunOption.RunNumber)+" ======================== Status = ",status
