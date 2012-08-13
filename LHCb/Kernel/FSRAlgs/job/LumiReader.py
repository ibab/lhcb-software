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
                           ]
# input file
files = [ "file:testout.mdf"]
ofilename='testout.dat'

LumiReader().inputFiles = files
LumiReader().outputFile =  ofilename
LumiReader().EvtMax =  20
LumiReader().Debug =  True 
LumiReader().OutputLevel =  DEBUG 

EventSelector().PrintFreq = 1
