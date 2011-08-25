#!/usr/bin/env gaudirun.py
#
import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp
from Configurables import CondDB

#-- set explicit CondDB tag
LHCbApp().CondDBtag = 'head-20110131'

#--- determine application to run
from Configurables import LumiAlgsConf, LumiCheckCondDB

# standard sequence from configurable
sequence = GaudiSequencer("CheckDB")

# normalization of BeamCrossing
seqMembers=[]
seqMembers.append( LumiCheckCondDB('CheckCondDB' ) )
from Configurables import GetLumiParameters
ToolSvc().addTool(GetLumiParameters, "lumiDatabaseTool") # This is a public tool
ToolSvc().lumiDatabaseTool.UseOnline = True
sequence.Members = seqMembers
sequence.MeasureTime = True
sequence.ModeOR = False
sequence.ShortCircuit = True
sequence.IgnoreFilterPassed = False

#-- main
ApplicationMgr( TopAlg = [ GaudiSequencer( "CheckDB"),
                           ], HistogramPersistency = 'NONE' )

#-- Test input not needed
files = [ ]

#-- input options
ApplicationMgr().EvtMax =  -1
importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
EventSelector( OutputLevel  = INFO, 
               PrintFreq    = 1000,
               FirstEvent   = 1,
               Input        = files
               )

