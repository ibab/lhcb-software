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

# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'NO prescale'

Moore().hltType = 'Hlt1' # +Hlt2'
Moore().verbose = True

#Moore().Simulation = False
#Moore().DataType = '2008'
#Moore().inputFiles= [ '/data/bfys/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw' ]

#Moore().Simulation = True
#Moore().DataType = 'DC06'
#Moore().inputFiles = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst'
#                     , '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst'
#                     , '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst'
#                     , '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

#Moore().Simulation = True
#Moore().DataType = 'DC06'
#Moore().inputFiles = [ '/data/bfys/lhcb/MinBias-L0strip/DC06_L0_v1_lumi2_MuonHadron_40000ev_' + str(f) +'.mdf'  for f in range(1,3) ]
#Moore().inputFiles = [ 'castor:/castor/cern.ch/user/s/snies/mdf/DC06_L0_v1_lumi2_MuonHadron_40000ev_'+str(f)+'.mdf' for f in range(1,3) ]
#Moore().inputFiles = [ '/data/bfys/lhcb/MinBias-L0strip/MBL0-lumi2-' + str(f) +'.dst'  for f in range(1,5) ]

Moore().Simulation = True
Moore().DataType   = '2008'
#Moore().inputFiles = [ '/data/bfys/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]
Moore().inputFiles = [ 'castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]

EventSelector().PrintFreq = 100

#Moore().DDDBtag  = "default"
#Moore().CondDBtag = "default"
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

# Moore().outputFile = '/tmp/foo.raw'

# optionally, we can enable some auditors...
# Moore().enableAuditor = [ NameAuditor() ]

#and tell the world how we are configured
print Moore()
