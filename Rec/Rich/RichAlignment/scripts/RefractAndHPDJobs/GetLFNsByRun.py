#! /usr/bin/env python

import pickle, bz2
import DIRAC

def usage():
  print 'Usage: %s <year> <startmonth> <startday> <lastmonth> <lastday>' %(Script.scriptName)
  
def getDate(year,month,day):
  return str(year)+'-'+str(month)+'-'+str(day)

from DIRAC.Core.Base import Script
Script.parseCommandLine()
args = Script.getPositionalArgs()
if len(args) < 5: 
  usage()
  DIRAC.exit(2)

# Get data base
from LHCbDIRAC.NewBookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
database = BookkeepingClient()

exitCode = 0

# Processing pass
procpass = 'Real Data'

year          = int(args[0])
startmonth    = int(args[1])
startday      = int(args[2])
lastmonth     = int(args[3])
lastday       = int(args[4])

if 2009 == year :
  ConfigV = ['Collision09']
elif 2010 == year :
  ConfigV = ['Collision10']
elif 2011 == year :
  ConfigV = ['Collision11','Collision11_25']
elif 2012 == year :
  ConfigV = ['Collision12']
else:
  print 'Unknown year', year
  DIRAC.exit(2)

allOK = True

# Dictionary for LFNs by Run
RunLFNs = { }

startDate = getDate(year,startmonth,startday)
endDate   = getDate(year,lastmonth,lastday)
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

  # Sort runs into numerical order
  runs.sort()

  # Loop over runs
  print runs
  nRun = 0
  for run in runs :
    nRun += 1

    for config in ConfigV :

      type = 91000000 # EXPRESS Stream
      if year == 2009 : type = 90000000 # Use full stream for 2009
      if run > 77595  and run < 77624  : # Express disappeared for unknown reasons
        type = 90000000 # FULL Stream
      if run > 100256 and run < 102177 : # Express turned off by accident after 09/2011 TS
        type = 90000000 # FULL Stream
      if config == 'Collision11_25' : # No express for 2011 25ns tests
        type = 90000000 # FULL Stream

      typeS = "EXPRESS"
      if type == 90000000 : typeS = "FULL"

      # Raw files
      bkDict = { 'ConfigName'           : 'LHCb',
                 'ConfigVersion'        : config,
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
        tmpLFNList.sort()
        if len(tmpLFNList) > 0 :
          if run not in RunLFNs.keys() : RunLFNs[run] = [ ]
          RunLFNs[run] += tmpLFNList

    nLFNs = 0
    if run in RunLFNs.keys() : nLFNs = len(RunLFNs[run])
    if nLFNs > 0 :
      print "(", nRun, "of", len(runs), ") Selected", nLFNs, "LFNs for run", run, ConfigV
    else:
      print "(", nRun, "of", len(runs), ") No data selected for run", run, ConfigV
        
if allOK :
    
  # Pickle the data
  if len(RunLFNs.keys()) > 0:
    filename = "RunData/RunLFNs_"+startDate+"_"+endDate+".pck.bz2"
    file = bz2.BZ2File(filename,"w")
    pickle.dump(RunLFNs,file)
    file.close()
    print "Created pickled data file", filename
  else:
    print "No Runs selected with data"

else:

  print "Problems getting data list. No pickled file produced ..."

# Return
DIRAC.exit(exitCode)
