#!/usr/bin/env python
# $Id: copy_files_to_db.py,v 1.2 2008-07-10 10:49:32 marcocle Exp $
import sys

import CondDBUI
import CondDBUI.Admin
from PyCool import cool

from optparse import OptionParser

import os, re

############################### connectStrings: ################################
# "oracle://intr1-v.cern.ch/INTR;schema=lhcb_cool;user=lhcb_cool;dbname=DDDB"
# "sqlite://none;schema=XmlDDDB_v26r2.db;user=none;password=none;dbname=DDDB"
# "mysql://pclhcb55.cern.ch;schema=MARCOCLE;user=marcocle;dbname=DDDB"
# "oracle://oradev10.cern.ch:10520/D10;schema=lhcb_lcg_cool_dev;user=lhcb_lcg_cool_dev;dbname=DDDB"
# "sqlite_file:XmlDDDB_v26r2.db/DDDB"
################################################################################
def main():
    parser = OptionParser()
    parser.add_option("-c", "--connect-string",
                      dest="connectString", type="string",
                      help="cool::DatabaseId to use for the connection [default: %default]",
                      default="sqlite:LocalDDDB.db/DDDB")
    parser.add_option("-s", "--source",
                      dest="source", type="string",
                      help="directory where the files to copy to the DB are",
                      metavar="SRCDIR")
    parser.add_option("-d", "--dest",
                      dest="dest", type="string",
                      help="directory, in the database, where to copy the files [default: %default]",
                      default="/",
                      metavar="DESTPATH")
    parser.add_option("-x", "--drop-db",
                      action="store_true", dest="drop",
                      help="drop the database before the copy [default: %default]",
                      default=False)
    parser.add_option("-X", "--noext",
                      action="store_true", dest="noextension",
                      help="Remove filename extension when creating a COOL Folder [default: %default]",
                      default=False)
    parser.add_option("", "--include-from-file",
                      dest="includeFile", type="string",
                      metavar="INCFILE",
                      help="store only the files included in the INCFILE")
#    parser.add_option("-k", "--keep-db",
#                      action="store_false", dest="drop",
#                      help="keep the existing database and merge with the new files (default)")

    (options, args) = parser.parse_args()
    if len(args) != 0 or not options.source or not options.dest:
        parser.error("Bad arguments, use -h for help.")

    if options.drop:
        CondDBUI.CondDB.dropDatabase(options.connectString)
    db = CondDBUI.CondDB(options.connectString, create_new_db = True, readOnly=False)

    includes = []
    if options.includeFile:
        includes = [ l.strip() for l in open(options.includeFile).xreadlines() ]
        
    CondDBUI.Admin.MakeDBFromFiles(options.source, db,
                                   includes = includes, excludes = [],
                                   verbose = True)
    
if __name__ == '__main__':
    main()
