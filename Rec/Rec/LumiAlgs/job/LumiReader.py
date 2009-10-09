#!/usr/bin/env gaudirun.py
#
# Minimal file for running LumiReader from python prompt
# Syntax is:
#   gaudirun.py ../job/LumiReader.py
# or just
#   ../job/LumiReader.py
#
import os
from Gaudi.Configuration import *
import Configurables as Configs
from Configurables import LHCbApp

#--- determine application to run
from LumiAlgs.LumiReaderConf import LumiReaderConf as LumiReader

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

# input file
files = [  
"file:testout.mdf",
#"rfio:/castor/cern.ch/user/p/panmanj/lumi/data/055153_0000000001.raw",
#"rfio:/castor/cern.ch/user/p/panmanj/lumi/data/055153_0000000003.raw",
]
ofilename='testout.dat'

LumiReader().inputFiles = files
LumiReader().outputFile =  ofilename
LumiReader().EvtMax =  20
LumiReader().Debug =  True # False
LumiReader().OutputLevel =  DEBUG #INFO # WARNING #DEBUG

EventSelector().PrintFreq = 1
