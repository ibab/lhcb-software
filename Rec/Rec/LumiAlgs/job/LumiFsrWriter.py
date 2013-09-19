#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiFsrWriter from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiFsrWriter.py
#
import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp
LHCbApp().XMLSummary = 'summary.xml'
#-- set explicit CondDB tag
LHCbApp().CondDBtag = 'cond-20130710'

#--- determine application to run
from LumiAlgs.LumiFsrWriterConf import LumiFsrWriterConf as LumiFsrWriter

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]
files = [ "lfn:/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/114753/114753_0000000015.raw"  ]
ofilename='PFN:testFSRout.dst'

LumiFsrWriter().inputFiles = files
LumiFsrWriter().outputFile =  ofilename
LumiFsrWriter().EvtMax =  1000
LumiFsrWriter().OutputLevel =  INFO
EventSelector().PrintFreq = 1000

#-- to test layer uncomment these lines
# from Configurables import CondDB
# CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
