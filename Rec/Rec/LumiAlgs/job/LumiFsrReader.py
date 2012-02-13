#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiFsrReader from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiFsrReader.py
# or just
#   ../job/LumiFsrReader.py
#
import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp
LHCbApp().XMLSummary = 'summary.xml'
#-- set explicit CondDB tag
# LHCbApp().CondDBtag = 'head-20100826'
LHCbApp().CondDBtag = 'head-20111111'


#--- determine application to run
from LumiAlgs.LumiFsrReaderConf import LumiFsrReaderConf as LumiFsrReader

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                            ]

files = [
  "PFN:testFSRout.dst"
  ]

LumiFsrReader().inputFiles = files
LumiFsrReader().EvtMax =  1000
LumiFsrReader().OutputLevel =  INFO
LumiFsrReader().DumpRequests =  "EFBC"

EventSelector().PrintFreq = 1000
