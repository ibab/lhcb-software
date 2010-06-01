#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiWriter from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiWriter.py
# or just
#   ../job/LumiWriter.py
#
import os
from Gaudi.Configuration import *
import Configurables as Configs
from Configurables import LHCbApp

#--- determine application to run
from LumiAlgs.LumiWriterConf import LumiWriterConf as LumiWriter

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                            ]

files = [
  "   DATAFILE='LFN:/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/71185/071185_0000000029.raw' SVC='LHCb::MDFSelector'"
  ]

ofilename='testout.mdf'

LumiWriter().inputFiles = files
LumiWriter().outputFile =  ofilename
LumiWriter().EvtMax =  1000
LumiWriter().Debug =  False 
LumiWriter().OutputLevel =  INFO

EventSelector().PrintFreq = 1000
