#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiWriter from python prompt
# Syntax is:
#   gaudirun.py ../job/Lumi0Writer.py
#
import os
from Gaudi.Configuration import *
import Configurables as Configs

#--- determine application to run
from LumiAlgs.LumiWriterConf import LumiWriterConf as LumiWriter

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                            ]
files = [
  #"   DATAFILE='LFN:/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/75710/075710_0000000180.raw' SVC='LHCb::MDFSelector'"
  "   DATAFILE='LFN:/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/80355/080355_0000000087.raw' SVC='LHCb::MDFSelector'"
  ]

ofilename='testout0.mdf'

LumiWriter().inputFiles = files
LumiWriter().outputFile =  ofilename
LumiWriter().EvtMax =  1000
LumiWriter().Debug =  False 
LumiWriter().OutputLevel =  INFO

EventSelector().PrintFreq = 1000

from Configurables import CondDB
CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
