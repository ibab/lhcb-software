#!/usr/bin/env python

from PyCool import cool
import os, sys

if len(sys.argv) > 1:
    connectionStrings = sys.argv[1:]
else:
    connectionStrings = [ os.environ["CONDDBCONNECTIONSTRING"] ]

for c in connectionStrings:
    cool.DatabaseSvcFactory.databaseService().createDatabase(c)

