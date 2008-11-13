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
#HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'
#---------------------------------------------------------------------------
# Number of events to process, optionally skipping some events
#---------------------------------------------------------------------------

# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'NO prescale'

Moore().hltType = 'Hlt1'
Moore().oldStyle = False
Moore().verbose = True

#tag = 'DC06-default'
#files= [ '/data/bfys/lhcb/MinBias-L0strip/DC06_L0_v1_lumi2_MuonHadron_40000ev_' + str(f) +'.mdf'  for f in range(1,3) ]

#tag = 'DC06-default'
#files = [ '/data/bfys/lhcb/MinBias-L0strip/MBL0-lumi2-' + str(f) +'.dst'  for f in range(1,5) ]

#tag = '2008-default'
#files= [ '/data/bfys/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw' ]

#tag = 'DC06-default'
#files = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

tag = 'DC06-default'
files = [ 'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_1.mdf',
          'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_2.mdf' ]



extensions = { 'RAW' : "' SVC='LHCb::MDFSelector'",
               'MDF' : "' SVC='LHCb::MDFSelector'",
               'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }
EventSelector().Input = [ "DATAFILE='PFN:"+ f + extensions[ files[0][-3:].upper() ] for f in files ]
EventSelector().PrintFreq = 100

Moore().DDDBtag   = tag
Moore().condDBtag = tag
Moore().EvtMax = 10000

# optionally, we can enable some auditors...
# Moore().enableAuditor = [ NameAuditor() ]
# always, always applyConf!
Moore().applyConf()

#and tell the world how we are configured
print Moore()
