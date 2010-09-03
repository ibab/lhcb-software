#! /usr/bin/env python

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

file = open(filename,"r")

import pickle
data = pickle.load(file)
 
file.close()

print data

