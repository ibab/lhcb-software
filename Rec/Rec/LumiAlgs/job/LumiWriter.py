#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiWriter from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiWriter.py
#
import os
from Gaudi.Configuration import *
import Configurables as Configs

#--- determine application to run
from LumiAlgs.LumiWriterConf import LumiWriterConf as LumiWriter

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                            ]
files = [ "lfn:/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/114753/114753_0000000016.raw"  ]
ofilename='testout.mdf'

LumiWriter().inputFiles = files
LumiWriter().outputFile =  ofilename
LumiWriter().EvtMax =  1000
LumiWriter().Debug =  False 
LumiWriter().OutputLevel =  INFO

EventSelector().PrintFreq = 1000

from Configurables import CondDB
CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
