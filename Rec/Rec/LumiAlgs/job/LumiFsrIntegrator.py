#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiFsrIntegrator from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiFsrIntegrator.py
# or just
#   ../job/LumiFsrIntegrator.py
#
import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp
LHCbApp().XMLSummary = 'summary.xml'

#--- determine application to run
from Configurables import LumiAlgsConf, DumpFSR
from LumiAlgs.LumiIntegratorConf import LumiIntegratorConf

# standard sequence from configurable
LumiAlgsConf().LumiSequencer = GaudiSequencer("LumiSeq", ShortCircuit = False )
LumiAlgsConf().InputType = 'DST'
LumiAlgsConf().OutputLevel =  INFO

# standard sequence from configurable
LumiIntegratorConf().LumiSequencer = GaudiSequencer("LumiIntSeq", ShortCircuit = False )

#-- main
ApplicationMgr( TopAlg = [ GaudiSequencer( "LumiSeq" ),
                           GaudiSequencer( "LumiIntSeq" ),
                           #DumpFSR()
                           ], HistogramPersistency = 'NONE' )

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]
#-- Test input
files = [ "DATAFILE='PFN:testFSRout.dst' TYP='POOL_ROOTTREE'  OPT='REC' " ]
#-- input options
ApplicationMgr().EvtMax =  1000
importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
EventSelector( OutputLevel  = INFO, 
               PrintFreq    = 1000,
               FirstEvent   = 1,
               Input        = files
               )

from Configurables import CondDB
CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
