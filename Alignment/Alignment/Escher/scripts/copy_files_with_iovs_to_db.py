#!/usr/bin/env python
# $Id: copy_files_with_iovs_to_db.py,v 1.1 2009-10-27 09:27:47 wouter Exp $
import sys

import CondDBUI
import CondDBUI.Admin
#from PyCool import cool

from optparse import OptionParser

import os, re

def main():
    parser = OptionParser()
 
# do we want to load the files on top of the snapshot? if so, make a snapshot

# first make a snap shot
#sys.exec('CondDBAdmin_MakeSnapshot.py -T head-20090928 LHCBCOND sqlite_file:VeloIOVTest.db/LHCBCOND')
connectString = 'sqlite_file:IOVTest.db/LHCBCOND'
os.system('CondDBAdmin_MakeSnapshot.py -T head-20091019 LHCBCOND ' + connectString )
     
# now read the input files and do something

#from CondDBAdmin_MakeSnapshot import main as CondDBAdmin_MakeSnapshot_main
inputfilename = sys.argv[1]
print 'Processing file: ', inputfilename

db = CondDBUI.CondDB(connectString, create_new_db = True, readOnly=False)

f=open(inputfilename, 'r')
lines = f.read().splitlines()
for line in lines:
    if len(line)<5 : continue
    if line[0]=='#': continue
    words = line.split(' ')
    directory = words[0].strip(' ')
    since     = words[1].strip(' ') 
    until     = words[2].strip(' ')

    print "Now inserting ",directory

    from PyCool import cool
    from GaudiPython.Bindings import gbl
    try:
        intsince = cool.ValidityKey(since)
    except ValueError:
        intsince = CondDBUI.Admin.timeToValKey( since, cool.ValidityKeyMin)
    try:
        intuntil = cool.ValidityKey(until)
    except ValueError:
        intuntil = CondDBUI.Admin.timeToValKey( until, cool.ValidityKeyMax)

    #intsince = CondDBUI.Admin.timeToValKey( since, cool.ValidityKeyMin)
    #intuntil = CondDBUI.Admin.timeToValKey( until, cool.ValidityKeyMax)
    print "From:  ", since, " --> ", intsince, " --> ", gbl.Gaudi.Time(intsince).format(True,"%c")
    print "Until: ", until, " --> ", intuntil, " --> ", gbl.Gaudi.Time(intuntil).format(True,"%c")
        
    CondDBUI.Admin.MakeDBFromFiles(directory, db,
                                   includes = [], excludes = [],
                                   verbose = True,
                                   since = intsince, until = intuntil
                                   )
