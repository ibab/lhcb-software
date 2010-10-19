#!/usr/bin/env gaudirun.py
#
# Syntax is:
#   gaudirun.py ../job/LumiCheckCondDB.py

import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp

#-- set explicit CondDB tag
LHCbApp().CondDBtag = 'head-20100826'

#--- determine application to run
from Configurables import LumiAlgsConf, LumiCheckCondDB

OutputLevel =  INFO

# standard sequence from configurable
sequence = GaudiSequencer("CheckDB")

# clock service for CondDB
#from Configurables import EventClockSvc
#EventClockSvc().EventTimeDecoder = "OdinTimeDecoder"

# normalization of BeamCrossing
seqMembers=[]
seqMembers.append( LumiCheckCondDB('CheckCondDB') )
sequence.Members = seqMembers
sequence.MeasureTime = True
sequence.ModeOR = False
sequence.ShortCircuit = True
sequence.IgnoreFilterPassed = False

#-- main
ApplicationMgr( TopAlg = [ GaudiSequencer( "CheckDB" ),
                           ], HistogramPersistency = 'NONE' )

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]
#-- Test input
files = [
  "DATAFILE='PFN:testFSRout.dst' TYP='POOL_ROOTTREE'  OPT='REC' ",
  "DATAFILE='PFN:testFSRout0.dst' TYP='POOL_ROOTTREE'  OPT='REC' ",
  ]
#-- input options
ApplicationMgr().EvtMax =  -1
importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
EventSelector( OutputLevel  = INFO, 
               PrintFreq    = 1000,
               FirstEvent   = 1,
               Input        = files
               )

#-- to test layer uncomment these lines
# from Configurables import CondDB
# CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
