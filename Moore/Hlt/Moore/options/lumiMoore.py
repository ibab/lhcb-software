#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
from Gaudi.Configuration import *
from Moore.Configuration import *

from lumiChecker import LumiAlg


#//---------------------------------------------------------------------------
#// Histogram output
#//---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

#---------------------------------------------------------------------------
# Number of events to process, optionally skipping some events
#---------------------------------------------------------------------------

# if you want to generate a configuration, uncomment the following line:
#Moore().GenerateConfig = True

# if you want to run, using a (set of) TCK(s), uncomment the following two
# lines, and input the (list of ) TCK(s)
#Moore().UseTCK = True
#Moore().PrefetchTCK = [ 0x00010000,0x00020000,0x00030000 ]
#HltConfigSvc().OutputLevel = DEBUG
#
jobType='readLumi'
#jobType='readRaw'

if jobType=='readRaw':
  Moore().hltType = 'Physics_Hlt1'
  Moore().hltType = 'Physics_Lumi'
  Moore().hltType = 'writeLumi'

  # the following gives the right db tag
  importOptions('$DDDBROOT/options/LHCb-2008.py')
  # only if writing a raw file
  if Moore().hltType == 'writeLumi':
    importOptions('WriteMDF.opts')

  # selection of files

  #files= [ '/data/bfys/lhcb/MinBias-L0strip/DC06_L0_v1_lumi2_MuonHadron_40000ev_' + str(f) +'.mdf'  for f in range(1,3) ]

  ## files = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst',
  ##           '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst',
  ##           '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst',
  ##           '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

  # the following file was triggered with the spd multiplicity (Patrick Robbe)
  files = [
      'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw',
      ]
  # the following should also contain SPD hits (hans)
  #files = [
  #    'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28578/028578_0000064259.raw',
  #    ]
  #files = [
  #    'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28578/028578_0000064262.raw',
  #    ]
  #files = [
  #    'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28578/028578_0000064263.raw',
  #    ]
  #files = [
  #    'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28578/028578_0000064267.raw',
  #    ]

elif jobType=='readLumi':
  Moore().hltType = 'readBackLumi'
  files = [ 'mdfData_run_2.mdf' ]   # this is a local test file of nano events
  

filetype = files[0][-3:].upper()

Moore().inputType = filetype

extensions = {
    'RAW' : "' SVC='LHCb::MDFSelector'",
    'MDF' : "' SVC='LHCb::MDFSelector'",
    'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }

EventSelector().Input =[ "DATAFILE='PFN:"+ f + extensions[ filetype ] for f in files ]
EventSelector().PrintFreq = 100

ApplicationMgr().EvtMax = 500

import GaudiPython
########################################
# bizarre: the following lines are needed
class AnyAlg(GaudiPython.PyAlgorithm):
  def __init__(self,name):
    pass
########################################

# instantiate the user algorithms
lumiAlg=LumiAlg('LumiAlgorithm')
#Moore().userAlgorithms=['LumiAlgorithm']
Moore().userAlgorithms=['LumiPrepareResult','LumiAlgorithm','LumiPutResult']

# optionally, we can enable some auditors...
# Moore().enableAuditor = [ NameAuditor() ]
# always, always applyConf!
Moore().applyConf()

# The user algorithm may need some knowledge about Gaudi
appMgr = GaudiPython.AppMgr()
lumiAlg.AppMgr = appMgr

# test prescaler
lumiAlg.PreScale=25 # in events
lumiAlg.Interval=100 # (in seconds)

# and tell the world how we are configured
print Moore()
