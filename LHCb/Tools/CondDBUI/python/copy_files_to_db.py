#!/usr/bin/env python
# $ Id: $
import sys

import conddbui
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
                      help="cool::DatabaseId to use for the connection",
                      default="sqlite://none;schema=sqlite.db;user=none;password=none;dbname=DDDB")
    parser.add_option("-s", "--source",
                      dest="source", type="string",
                      help="directory where the files to copy to the DB are",
                      metavar="SRCDIR")
    parser.add_option("-d", "--dest",
                      dest="dest", type="string",
                      help="directory, in the database, where to copy the files (default: '/')",
                      default="/",
                      metavar="DESTPATH")
    parser.add_option("-x", "--drop-db",
                      action="store_true", dest="drop",
                      help="drop the database before the copy",
                      default=False)
    parser.add_option("-X", "--noext",
                      action="store_true", dest="noextension",
                      help="Remove filename extension when creating a COOL Folder",
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
        conddbui.CondDB.dropDatabase(options.connectString)
    db = conddbui.CondDB(options.connectString, create_new_db = True, readOnly=False)

    # prepare the list of nodes to insert
    # the format is something like:
    # {
    #   "/path/to/folderset" : {
    #                           "folder.xml" : {
    #                                           "data": { 0 : "filename",
    #                                                     ...
    #                                                   },
    #                                           ...
    #                                          },
    #                           ...
    #                          },
    #   ...
    # }
    includes = []
    if options.includeFile:
        includes = [ l.strip() for l in open(options.includeFile).xreadlines() ]
    nodes = conddbui._collect_tree_info(options.source, includes = includes, excludes = [])

    # Just count the number of folders we are goinfg to write
    count_folders = 0
    for folderset in nodes:
        count_folders += len(nodes[folderset])
    count_folders_len = str(len(str(count_folders)))

    # initialize conters for progress output
    file_count = 0
    folder_count = 0
    foldersets = nodes.keys()
    foldersets.sort() # not needed, but it seems more clean
    
    for folderset in foldersets:
        
        folders = nodes[folderset].keys()
        folders.sort()

        for folder in folders:
            keys = nodes[folderset][folder].keys()
            keys.sort()
            
            folder_path = re.sub('/+','/','/'.join([options.dest,folderset,folder]))
            if options.noextension:
                folder_path = os.path.splitext(folder_path)[0]

            if not db.db.existsFolder(folder_path):
                db.createNode(path = folder_path, storageKeys = keys)
                
            collection = {}
            for key in keys:
                for channel in nodes[folderset][folder][key].keys():
                    
                    if channel not in collection:
                        collection[channel] = {}
                        for k in keys:
                            collection[channel][k] = ""
            
                    collection[channel][key] = conddbui._fix_xml(open(nodes[folderset][folder][key][channel],"rb").read(),
                                                                 "conddb:"+'/'.join([options.dest,folderset]))
                    
            xmllist = []
            for channel in collection:
                xmllist.append({ 'payload': collection[channel],
                                 'since': cool.ValidityKeyMin,
                                 'until': cool.ValidityKeyMax,
                                 'channel': channel })
                
            folder_count += 1
            file_count += len(keys)
            print ("%" + count_folders_len + "d %" + count_folders_len + "d  %s (%d files)")%\
                  (folder_count,count_folders-folder_count,folder_path,len(keys))
            db.storeXMLStringList(folder_path, xmllist)

    print "Total folders inserted = %d"%folder_count
    print "Total files inserted = %d"%file_count

if __name__ == '__main__':
    main()
