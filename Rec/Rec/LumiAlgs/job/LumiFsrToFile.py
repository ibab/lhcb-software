#!/usr/bin/env gaudirun.py
#
# Syntax is:
#   gaudirun.py ../job/LumiFsrToFile.py
#
import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp
LHCbApp().XMLSummary = None

#--- determine application to run
from LumiAlgs.LumiFsrReaderConf import LumiFsrReaderConf as LumiFsrReader

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           ]

files = [
  "DATAFILE='PFN:testFSRout.dst' TYP='POOL_ROOTTREE' ",
  ]

LumiFsrReader().inputFiles = files
LumiFsrReader().EvtMax =  -1
LumiFsrReader().OutputLevel =  WARNING
LumiFsrReader().asciiFile =  'test_ascii_fsr.dat'

EventSelector().PrintFreq = 1000
