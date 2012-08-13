#!/usr/bin/env gaudirun.py
#
# Syntax is:
#   gaudirun.py ../job/LumiCheckCondDB.py

import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp
from Configurables import CondDB

# use oracle + grid certificate (not in unittests)
# CondDB(UseOracle = True)
# CondDB().LocalTags["LHCBCOND"] = ["lumi-20101026"]

#-- set explicit CondDB tag
#LHCbApp().CondDBtag = 'head-20100826'
#LHCbApp().CondDBtag = 'lumi-20101026'
LHCbApp().CondDBtag = 'lumi-20110131'

#--- determine application to run
from Configurables import LumiAlgsConf, LumiCheckCondDB, GetLumiParameters

# standard sequence from configurable
sequence = GaudiSequencer("CheckDB")

# normalization of BeamCrossing
seqMembers=[]
lcc = LumiCheckCondDB('CheckCondDB' )
ToolSvc().addTool(GetLumiParameters, "lumiDatabaseTool")
ToolSvc().lumiDatabaseTool.UseOnline = False 
seqMembers.append( lcc )
sequence.Members = seqMembers
sequence.MeasureTime = True
sequence.ModeOR = False
sequence.ShortCircuit = True
sequence.IgnoreFilterPassed = False

#-- main
ApplicationMgr( TopAlg = [ GaudiSequencer( "CheckDB"),
                           ], HistogramPersistency = 'NONE' )

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]
#-- Test input not needed
files = [ ]

#-- input options
ApplicationMgr().EvtMax =  -1

EventSelector( OutputLevel  = INFO, 
               PrintFreq    = 1000,
               FirstEvent   = 1,
               Input        = files
               )

#-- to test layer uncomment these lines
from Configurables import CondDB
CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
