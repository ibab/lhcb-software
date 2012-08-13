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

# files = [ "DATAFILE='mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/freezer/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/81430/081430_0000000113.raw' SVC='LHCb::MDFSelector'"  ]
files = [ "lfn:lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/81430/081430_0000000113.raw"  ]

ofilename='testout0.mdf'

LumiWriter().inputFiles = files
LumiWriter().outputFile =  ofilename
LumiWriter().EvtMax =  1000
LumiWriter().Debug =  False 
LumiWriter().OutputLevel =  INFO

EventSelector().PrintFreq = 1000

#-- to test layer 
from Configurables import CondDB
CondDB().addLayer(dbFile = "LHCBCOND_Lumi_IOV.db", dbName = "LHCBCOND")
