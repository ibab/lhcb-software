#!/usr/bin/env python
__author__  = "Wouter Hulsbergen"
__version__ = "$Id: copy_files_with_iovs_to_db.py,v 1.2 2009/10/27 16:12:44 wouter Exp $"

import sys

import CondDBUI
import CondDBUI.Admin
#from PyCool import cool

from optparse import OptionParser

import os, re

def main():
    parser = OptionParser(usage = "%prog [options] iovfile",
                          version = __version__,
                          description =
"""This script produces SQLite data base from a text file with directory names
and validity intervals. Each line in the input file should look like \n
   directoryname : begin-iov : end-iov \n
for example \n
   NewAlignment : 2009-10-22_10:48:00 : 2009-10-22_10:53:15 \n
Unless '-n' is specified, the script first makes a snapshot of the default databse using
CondDBAdmin_MakeSnapshot.py. This is essential if you want to use the result as a layer.""")
    parser.add_option("-s", "--sqlitefilename",
                      dest="sqlitefilename", type="string",
                      help="name of the output sqlitefile [default: %default]",
                      default="LocalDDDB.db")
    parser.add_option("-p", "--partition",
                      dest="partition", type="string",
                      help="partition [default: %default]",
                      default="LHCBCOND")
    parser.add_option("-n", "--no-snapshot", action="store_true",
                      dest="doNotMakeSnapShot", 
                      help="do not create a snapshot before adding the files",
                      default=False)
    parser.add_option("-T", "--tag",
                      action="store_true", dest="tag",
                      help="tag for the snapshot [default: %default]",
                      default="HEAD")

    (opts, args) = parser.parse_args()
    
    # check arguments
    if len(args) != 1:
        parser.error("not enough arguments. Try with --help.")

    # do we want to load the files on top of the snapshot? if so, make a snapshot
    # first make a snap shot #sys.exec('CondDBAdmin_MakeSnapshot.py -T head-20090928 LHCBCOND sqlite_file:VeloIOVTest.db/LHCBCOND')
    connectString = "sqlite_file:" + opts.sqlitefilename + "/" + opts.partition
    if not opts.doNotMakeSnapShot:
        #from CondDBAdmin_MakeSnapshot import main as CondDBAdmin_MakeSnapshot_main
        # remove database if it exists
        os.system('rm -f ' + opts.sqlitefilename)
        # create a new one
        os.system('CondDBAdmin_MakeSnapshot.py -T ' + opts.tag + ' ' + opts.partition + ' ' +  connectString)
             
    # now read the input files and do something
    inputfilename = sys.argv[1]
    inputfilename = args[0]
    print 'Processing file: ', inputfilename

    db = CondDBUI.CondDB(connectString, create_new_db = True, readOnly=False)

    f=open(inputfilename, 'r')
    lines = f.read().splitlines()
    for line in lines:
        if len(line)<5 : continue
        if line[0]=='#': continue
        words = line.split(' : ')
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
    # make sure to tag everything
    if opts.tag != 'HEAD':
        db.recursiveTag('/',opts.tag)

if __name__ == '__main__':
    main()
