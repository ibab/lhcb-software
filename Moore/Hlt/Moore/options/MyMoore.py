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

# if you want to generate a configuration, uncomment the following line:
#Moore().GenerateConfig = True

# if you want to run, using a (set of) TCK(s), uncomment the following two
# lines, and input the (list of ) TCK(s)
#Moore().UseTCK = True
#Moore().PrefetchTCK = [ 0x00010000,0x00020000,0x00030000 ]
#HltConfigSvc().OutputLevel = DEBUG
#
Moore().hltType = 'Physics_Hlt1'
Moore().hltType = 'Physics_Lumi'
Moore().hltType = 'Lumi'
#Moore().hltType = 'Physics_Hlt1+Hlt2'
#Moore().DDDBtag = 'head-20080422'
#Moore().condDBtag = 'head-20080422'
## Moore().DDDBtag       = "2008-default"
## Moore().condDBtag    = "2008-default"
# the following gives the right db tag
importOptions('$DDDBROOT/options/LHCb-2008.py') 

#files= [ '/data/bfys/lhcb/MinBias-L0strip/DC06_L0_v1_lumi2_MuonHadron_40000ev_' + str(f) +'.mdf'  for f in range(1,3) ]

## files = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst',
##           '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst',
##           '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst',
##           '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

files = [ 'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_1.mdf',
          'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_2.mdf' ]

# without L0DU
#files = [ '/castor/cern.ch/grid/lhcb/data/2008/RAW/MUON/PHYSICS/27331/027331_0000062878.raw' ]

# with L0DU
#files = [ 'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27041/027041_0000062711.raw']
## files = [
##     'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28618/028618_0000064270.raw',
##     'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28618/028618_0000064278.raw',
##     ]
## files = [
##     'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28640/028640_0000064288.raw',
##     ]
files = [
    'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/28643/028643_0000064291.raw',
    ]
# the following file was triggered with the spd multiplicity (Patrick Robbe)
files = [
    'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw',
    ]
# the following should also contain hits (hans)
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
## 028578_0000064259.raw
## 028578_0000064262.raw
## 028578_0000064263.raw
## 028578_0000064267.raw

filetype = files[0][-3:].upper()

Moore().inputType = filetype

extensions = {
    'RAW' : "' SVC='LHCb::MDFSelector'",
    'MDF' : "' SVC='LHCb::MDFSelector'",
    'DST' : "' TYP='POOL_ROOTTREE' OPT='READ'" }

EventSelector().Input =[ "DATAFILE='PFN:"+ f + extensions[ filetype ] for f in files ]
EventSelector().PrintFreq = 10

ApplicationMgr().EvtMax = 10

import GaudiPython
########################################
class MyAlg(GaudiPython.PyAlgorithm):
  def __init__ ( self , name ) :
    """ Constructor """
    GaudiPython.PyAlgorithm.__init__( self , name )
    self.name=name
    print self.name,': __init__'

  def initialize(self):
    self.nevsel=0
    print self.name,': counter=',self.nevsel
    return True

  def reinitialize(self):
    return True
  
  def finalize(self):
    print self.name,': counter=',self.nevsel
    return True
        
  def execute(self):
    self.nevsel+=1
    print self.name,': counter+',self.nevsel

    return True
########################################
myAlg=MyAlg('MyAlg')
Moore().userAlgorithms=['MyAlg']

# optionally, we can enable some auditors...
# Moore().enableAuditor = [ NameAuditor() ]
# always, always applyConf!
Moore().applyConf()

#and tell the world how we are configured
print Moore()
