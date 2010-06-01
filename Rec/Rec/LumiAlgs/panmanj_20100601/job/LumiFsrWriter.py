#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiFsrWriter from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiFsrWriter.py
# or just
#   ../job/LumiFsrWriter.py
#
import os
from Gaudi.Configuration import *

#--- switch on xml summary
from Configurables import LHCbApp
LHCbApp().XMLSummary = 'summary.xml'

#--- determine application to run
from LumiAlgs.LumiFsrWriterConf import LumiFsrWriterConf as LumiFsrWriter

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                            ]
files = [
  "   DATAFILE='LFN:/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/71185/071185_0000000029.raw' SVC='LHCb::MDFSelector'"
  ]

ofilename='testFSRout.dst'

LumiFsrWriter().inputFiles = files
LumiFsrWriter().outputFile =  ofilename
LumiFsrWriter().EvtMax =  1000
LumiFsrWriter().OutputLevel =  INFO

EventSelector().PrintFreq = 1000
