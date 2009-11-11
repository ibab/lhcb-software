#!/usr/bin/env gaudirun.py
#
# Place holder for testing the FSR info from a DST
# At the moment I don't know quite what to write :S
#
import os
from Gaudi.Configuration import *
import Configurables as Configs
from Configurables import LHCbApp

#--- determine application to run
from LumiAlgs.LumiWriterConf import LumiWriterConf as LumiWriter

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

# input file
files = [  
"rfio:/castor/cern.ch/user/p/panmanj/lumi/data/055153_0000000001.raw",
"rfio:/castor/cern.ch/user/p/panmanj/lumi/data/055153_0000000003.raw",
]
ofilename='testout.mdf'

LumiWriter().inputFiles = files
LumiWriter().outputFile =  ofilename
LumiWriter().EvtMax =  1000
LumiWriter().Debug =  False # True
LumiWriter().OutputLevel =  WARNING # INFO # DEBUG

EventSelector().PrintFreq = 1000
