#! /usr/bin/env python

import pickle, bz2
import DIRAC

def usage():
  print 'Usage: %s <year> <startmonth> <startday> <lastmonth> <lastday> <maxrun - optional>' %(Script.scriptName)
  
def getDate(year,month,day):
  return str(year)+'-'+str(month)+'-'+str(day)

from DIRAC.Core.Base import Script
Script.parseCommandLine()
args = Script.getPositionalArgs()
if len(args) < 5: 
  usage()
  DIRAC.exit(2)

# Get data base
from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
database = BookkeepingClient()

exitCode = 0

year          = int(args[0])
startmonth    = int(args[1])
startday      = int(args[2])
lastmonth     = int(args[3])
lastday       = int(args[4])
maxrun        = -1
if len(args) >= 6 : maxrun = int(args[5])

if 2009 == year :
  ConfigV = ['Collision09']
elif 2010 == year :
  ConfigV = ['Collision10']
elif 2011 == year :
  ConfigV = ['Collision11','Collision11_25']
elif 2012 == year :
  ConfigV = ['Collision12','Protonion12','Collision12hl','Collision12_25']
elif 2013 == year :
  ConfigV = ['Collision13','Protonion13','Ionproton13','Ionsmog']
elif 2015 == year :
  ConfigV = ['Collision15','Collision15em']
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
         'AllowOutsideRuns' : True, # Allows runs to start inside but finish outside the dates
         'CheckRunStatus'   : False # Don't care if it is processed or not
         }
print "Getting Runs with", dict
resultA = database.getRunsForAGivenPeriod(dict)

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

    if maxrun == -1 or run <= maxrun :
      
      nRun += 1

      for config in ConfigV :

        type = 91000000 # EXPRESS Stream
        if year == 2009 : type = 90000000 # Use full stream for 2009
        if run > 71473  and run < 72332 : # Use FULL for first 14nb-1 in 2010
          type = 90000000 # FULL Stream
        if run > 77595  and run < 77624 : # Express disappeared for unknown reasons
          type = 90000000 # FULL Stream
        if run > 100256 and run < 102177 : # Express turned off by accident after 09/2011 TS
          type = 90000000 # FULL Stream
        if config == 'Collision11_25' : # No express for 2011 25ns tests
          type = 90000000 # FULL Stream
        if config == 'Protonion12' : # (Currently) no express stream for pA data
          type = 90000000 # FULL Stream
        if config == 'Protonion13' or config == 'Ionproton13' :
          type = 90000000 # FULL Stream
        #if  run > 77595  and run < 77624 : # Express disappeared for unknown reasons
        if run > 157596 : # Express stream turned off in Run II
          type = 90000000 # FULL Stream

        typeS = "EXPRESS"
        if type == 90000000 : typeS = "FULL"

        # Processing pass. For 2011 express stream data used a Merged location
        procpass = 'Real Data'
        if year == 2011 and type != 90000000 :
          procpass = 'Real Data/Merging'

        # Raw files
        bkDict = { 'ConfigName'        : 'LHCb',
                   'ConfigVersion'     : config,
                   'ProcessingPass'    : procpass,
                   'FileType'          : 'ALL',
                   'StartRun'          : run,
                   'EndRun'            : run,
                   'EventType'         : type }
        resultB = database.getFilesWithGivenDataSets(bkDict)

        if not resultB['OK']:
          print "Error querying BK with", bkDict, resultB['Message']
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

      if not allOK : break
            
      nLFNs = 0
      if run in RunLFNs.keys() : nLFNs = len(RunLFNs[run])
      if nLFNs > 0 :
        print "(", nRun, "of", len(runs), ") Selected", nLFNs, "LFNs for run", run, ConfigV
        print RunLFNs[run]
      else:
        print "(", nRun, "of", len(runs), ") No data selected for run", run, ConfigV

    else:

      print "Skipping run", run
        
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
