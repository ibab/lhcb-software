# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore2.py
#
from Gaudi.Configuration import *
from Moore.Configuration import *


# General output levels 
ToolSvc().OutputLevel                     = INFO
from Configurables import XmlParserSvc
XmlParserSvc().OutputLevel                = WARNING
MessageSvc().OutputLevel                  = INFO
ApplicationMgr().OutputLevel              = ERROR

# Print algorithm name with 40 characters
MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'

#//---------------------------------------------------------------------------
#// Histogram output
#//---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

#//---------------------------------------------------------------------------
#// Number of events to process, optionally skipping some events
#//---------------------------------------------------------------------------
ApplicationMgr().EvtMax =  10000 ;

# if you want to generate a configuration, uncomment the following line:
#Moore().GenerateConfig = True

# if you want to run, using a (set of) TCK(s), uncomment the following two
# lines, and input the (list of ) TCK(s)
#Moore().UseTCK = True
#Moore().PrefetchTCK = [ 3 ]

# always applyConf!
Moore().applyConf()
print Moore()

#files= [ '/data/bfys/lhcb/MinBias-L0strip/MBL0-lumi2-' + str(f) +'.dst'  for f in range(1,5) ]

files = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst',
          '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst',
          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst',
          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

EventSelector().Input =[ "DATAFILE='PFN:"+ f +"' TYP='POOL_ROOTTREE' OPT='READ'" for f in files ]
EventSelector().PrintFreq = 100;
