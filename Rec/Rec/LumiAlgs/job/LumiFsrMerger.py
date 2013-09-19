#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiFsrMerger from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiFsrMerger.py
# or just
#   ../job/LumiFsrMerger.py
#
import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp

LHCbApp().XMLSummary = 'summary.xml'
#-- set explicit CondDB tag
LHCbApp().CondDBtag = 'cond-20130710'

#--- determine application to run
from LumiAlgs.LumiMergeFSRConf import LumiMergeFSRConf

# standard sequence from configurable
LumiMergeFSRConf().LumiSequencer = GaudiSequencer("LumiMergeSeq", ShortCircuit = False )
LumiMergeFSRConf().outputFile = 'mergeout.dst' 

#-- main
ApplicationMgr( TopAlg = [ GaudiSequencer( "LumiMergeSeq" ),
                           ], HistogramPersistency = 'NONE' )

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]
#-- Test input
files = [
  "DATAFILE='PFN:testFSRout.dst' TYP='POOL_ROOTTREE'  OPT='REC' " ,
  "DATAFILE='PFN:testFSRout0.dst' TYP='POOL_ROOTTREE'  OPT='REC' " ,
  ]
#-- input options
ApplicationMgr().EvtMax =  -1
EventSelector( OutputLevel  = INFO, 
               PrintFreq    = 1000,
               Input        = files
               )

#-- to test layer uncomment these lines
# from Configurables import CondDB
# CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
