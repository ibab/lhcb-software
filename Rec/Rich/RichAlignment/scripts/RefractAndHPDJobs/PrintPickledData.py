#! /usr/bin/env python

import pickle, bz2

def usage():
  print 'Usage: %s <datafile>' %(Script.scriptName)

from DIRAC.Core.Base import Script
Script.parseCommandLine()
args = Script.getPositionalArgs()
if len(args) < 1: 
  usage()
  import DIRAC
  DIRAC.exit(2)

filename = str(args[0])

file = bz2.BZ2File(filename,"r")

data = pickle.load(file)
 
file.close()

print data

