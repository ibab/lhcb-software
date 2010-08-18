#! /usr/bin/env python

import pickle
import DIRAC

def usage():
  print 'Usage: %s <year> <month> <firstday> <lastday>' %(Script.scriptName)
  
def getDate(year,month,day):
  return str(year)+'-'+str(month)+'-'+str(day)

from DIRAC.Core.Base import Script
Script.parseCommandLine()
args = Script.getPositionalArgs()
if len(args) < 4: 
  usage()
  DIRAC.exit(2)

# Get data base
from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
database = BookkeepingClient()

exitCode = 0

# Processing pass
procpass = 'Real Data'

year     = int(args[0])
month    = int(args[1])
startday = int(args[2])
endday   = int(args[3])

firstDate = getDate(year,month,startday)
lastDate  = getDate(year,month,endday)
print "Creating run list for period",firstDate,"to",lastDate

allOK = True

# Dictionary for LFNs by Run
RunLFNs = { }

for day in range(startday,endday):

  startDate = getDate(year,month,day)
  endDate   = getDate(year,month,day+1)
  
  dict = { 'StartDate'        : startDate,
           'EndDate'          : endDate,
           'AllowOutsideRuns' : True # Allows run to start inside but finish outside the dates
           }
  print "Getting Runs with", dict
  resultA = database.getRunsWithAGivenDates(dict)

  if not resultA['OK']:
  
    print 'ERROR %s' % (resultA['Message'])
    exitCode = 2
    # Return
    DIRAC.exit(exitCode)
  
  else:

    runs = resultA['Value']['Runs']
    print runs
    
    # Sort runs into numerical order
    runs.sort()

    # Loop over runs
    for run in runs :

      type = 91000000 # EXPRESS Stream
      if run > 77595 and run < 77624 :
        print "Warning : run", run, "had no EXPRESS stream :( Using FULL instead"
        type = 90000000 # FULL Stream
  
      # Raw files
      bkDict = {'ConfigName'           : 'LHCb',
                'ConfigVersion'        : 'Collision10',
                'ProcessingPass'       : procpass,
                'FileType'             : 'ALL',
                'StartRun'             : run,
                'EndRun'               : run,
                'EventType'            : type }
      resultB = database.getFilesWithGivenDataSets(bkDict)
      if not resultB['OK']:
        print resultB['Message']
        allOK = False
      else:
        tmpLFNList = [ ]
        for lfn in resultB['Value']:
          filetype = lfn.split('.')[1]
          if filetype == 'dst' or filetype == 'raw':
            tmpLFNList += ["LFN:"+lfn]
        if len(tmpLFNList) > 0 :
          RunLFNs[run] = tmpLFNList
          print "Selected", len(RunLFNs[run]), "LFNs for run", run
        else:
          print "No data selected for run", run

if allOK :
    
  # Pickle the data
  if len(RunLFNs.keys()) > 0:
    filename = "RunData/RunLFNs_"+firstDate+"_"+lastDate+".pck"
    file = open(filename,"w")
    pickle.dump(RunLFNs,file)
    file.close()
    print "Created pickled data file", filename
  else:
    print "No Runs selected with data"

else:

  print "Problems getting data list. No pickled file produced ..."

# Return
DIRAC.exit(exitCode)
