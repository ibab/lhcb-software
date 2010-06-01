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

#--- determine application to run
from LumiAlgs.LumiFsrReaderConf import LumiFsrReaderConf as LumiFsrReader

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                            ]

files = [
  "DATAFILE='PFN:testFSRout.dst' TYP='POOL_ROOTTREE' "
  ]

LumiFsrReader().inputFiles = files
LumiFsrReader().EvtMax =  1000
LumiFsrReader().OutputLevel =  INFO

EventSelector().PrintFreq = 1000
