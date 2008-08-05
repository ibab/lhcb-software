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


#//---------------------------------------------------------------------------
#// Histogram output
#//---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

#---------------------------------------------------------------------------
# Number of events to process, optionally skipping some events
#---------------------------------------------------------------------------

# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'standard_hlt1'

# if you want to run, using a (set of) TCK(s), uncomment the following two
# lines, and input the (list of ) TCK(s)
#Moore().useTCK = True
#Moore().prefetchTCK = [ 0x00100000 ]
#HltConfigSvc().OutputLevel = DEBUG
#HltConfigSvc().checkOdin = False
#
Moore().runType = 'Physics_Hlt1'
#Moore().runType = 'Physics_Hlt1+Hlt2'
#Moore().runType = 'Commissioning'

#Moore().DDDBtag = '2008-default'
#Moore().condDBtag = '2008-default'

#files= [ '/data/bfys/lhcb/MinBias-L0strip/DC06_L0_v1_lumi2_MuonHadron_40000ev_' + str(f) +'.mdf'  for f in range(1,3) ]

#files= [ '/data/bfys/lhcb/MinBias-L0strip/MBL0-lumi2-' + str(f) +'.dst'  for f in range(1,5) ]

#files = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

files = [ 'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_1.mdf',
          'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_2.mdf' ]


filetype = files[0][-3:].upper()
Moore().inputType = filetype
extensions = { 'MDF' : "' SVC='LHCb::MDFSelector'",
               'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }
EventSelector().Input = [ "DATAFILE='PFN:"+ f + extensions[ filetype ] for f in files ]

EventSelector().PrintFreq = 100

Moore().EvtMax = 10000


# optionally, we can enable some auditors...
# Moore().enableAuditor = [ NameAuditor() ]
# always, always applyConf!
Moore().applyConf()


#and tell the world how we are configured
print Moore()
