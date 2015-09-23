import sys
args=sys.argv[:]
import RunOption
import HLT2Params
import sys
import os
print "===================== Adding the Run Tick to the Database ========================"
import CondDBUI
import CondDBUI.Admin
DBString = "CondDBOnline(owner)/ONLINE" # use the central Oracle Database...
db_doneFile = open(RunOption.OutputDirectory+"/../CondDBXfer_done","w")
db_doneFile.write(str(HLT2Params.RunStartTime)+" "+str(HLT2Params.RunDuration))
db_doneFile.close()
print "Adding the RunTick to the Database"
print "IoV is ["+str(RunStart)+","+str(RunEnd)+"]"+", ["+time.ctime(HLT2Params.RunStartTime)+","+time.ctime(RunEnd/1000000000)+"]"
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=False)
status = CondDBUI.Admin.MakeDBFromFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd,
                                   writeDuplicate = False
                                    )
db.closeDatabase()
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=True)
print "Checking the Run Tick"
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd
                                    )
if Fails:
  print "Readback of Run Tick check on DB failed\n"
  pprint(Fails)
  sys.exit(44)
db.closeDatabase()
DBString = "CondDBOnline/ONLINE" # use the central Oracle Database...
db = CondDBUI.CondDB(DBString, create_new_db = False, readOnly=True)
print "Checking Database Contents against entered files with account "+DBString
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/offl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd
                                    )
if Fails:
  print "Readback check on DB failed\n"
  pprint(Fails)
  sys.exit(44)
print "Checking The Run Tick contents with account "+DBString
Fails = CondDBUI.Admin.CompareDBToFiles(RunOption.OutputDirectory+"/onl", db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = RunStart,
                                   until = RunEnd
                                    )
if Fails:
  print "Readback check on DB failed\n"
  pprint(Fails)
  sys.exit(44)

print "===================== Updated the Database ======================== Status = ",status
