#!/usr/bin/env python
from PyCool import cool

from optparse import OptionParser
import os
import re

sysIdRE = re.compile('SYSTEM[^>"\']*("[^">]*"|'+"'[^'>]*')")
def fix_system_ids(xml_data,path):
    data = xml_data
    m = sysIdRE.search(data)
    while m != None:
        pos = m.start()
        s = m.start(1)+1
        e = m.end(1)-1
        p = os.path.join(path,data[s:e])
        p = os.path.normpath(p)
        data = data[0:s] + p + data[e:] 
        m = sysIdRE.search(data,pos+1)
    return data

envVarRE = re.compile('\$([A-Za-z][A-Za-z0-9_]*)')
#cvs_vars = [ 'Id', 'Name', 'Log' ]
def fix_env_vars(xml_data):
    data = xml_data
    m = envVarRE.search(data)
    while m != None:
        pos = m.start()
        s = m.start(1)
        e = m.end(1)
        name = m.group(1)
        
        if os.environ.has_key(name):
            val = os.environ[name]
        else:
            val = name
            
        data = data[0:pos] + val + data[e:]
        m = envVarRE.search(data,pos+1)
    return data

#### connectStrings: ####
# "oracle://intr1-v.cern.ch/INTR;schema=lhcb_cool;user=lhcb_cool;dbname=DDDB"
# "sqlite://none;schema=XmlDDDB_v26r2.sqlite;user=none;password=none;dbname=DDDB"
# "mysql://pclhcb91.cern.ch;schema=MARCOCLE;user=marcocle;dbname=DDDB"
# "oracle://oradev10.cern.ch:10520/D10;schema=lhcb_lcg_cool_dev;user=lhcb_lcg_cool_dev;dbname=DDDB"

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
                      help="drop the database before the copy (default)",
                      default=True)
    parser.add_option("-k", "--keep-db",
                      action="store_false", dest="drop",
                      help="keep the existing database and merge with the new files")
    (options, args) = parser.parse_args()
    if len(args) != 0 or not options.source or not options.dest:
        parser.error("Bad arguments, use -h for help.")

    #### prepare structres to create and fill the folders
    folderspec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION)
    #folderspec.payloadSpecification().extend("data",cool.StorageType.String255)
    #folderspec.payloadSpecification().extend("data",cool.StorageType.String4k)
    #folderspec.payloadSpecification().extend("data",cool.StorageType.String64k)
    folderspec.payloadSpecification().extend("data",cool.StorageType.String16M)
    folderdesc = "<storage_type=7>"

    payload = cool.Record(folderspec.payloadSpecification())
    payload["data"] = "XML stuff" # dummy value, just to start
    
    #### initialize COOL
    dbs = cool.DatabaseSvcFactory.databaseService()

    if options.drop:
        dbs.dropDatabase(options.connectString)
        db = dbs.createDatabase(options.connectString)
    else:
        print 
        db = dbs.openDatabase(options.connectString, False)
    
    excludes = [ 'clhep2dtd.pl' ]
    
    # count files (just to know)
    sum = 0
    for root, dirs, files in os.walk(options.source):
        if 'CVS' in root: # skip CVS directories
            continue
        for x in excludes:
            if x in files:
                files.remove(x)
        sum += len(files)

    num_len = len(str(sum))
    # populate the database
    n = 0
    for root, dirs, files in os.walk(options.source):
        # remove base path
        dbroot = root.replace(options.source,options.dest)
        dbroot = dbroot.replace('//','/')
        
        if 'CVS' in root: # skip CVS directories
            continue
    
        if dbroot != "/": # it means it is not "/"
            if not db.existsFolderSet(dbroot):
                db.createFolderSet(dbroot)
        #else:
        #    dbroot = "/"
        
        print dbroot
        for x in excludes:
            if x in files:
                files.remove(x)
        for f in files:
            n+=1
            print ("%" + str(num_len) + "d %" + str(num_len) + "d  %s")%(n,sum-n,f)
            folder_path = os.path.join(dbroot,f)
            if db.existsFolder(folder_path):
                folder = db.getFolder(folder_path)
            else:
                folder = db.createFolder(folder_path,
                                         folderspec,folderdesc)
            xml_data = open(os.path.join(root,f)).read()
            fixed_data = fix_system_ids(xml_data,"conddb:"+dbroot)
            #fixed_data = xml_data
            fixed_data = fix_env_vars(fixed_data)
            payload["data"] = fixed_data
            folder.storeObject(cool.ValidityKeyMin,cool.ValidityKeyMax,payload,0) 

    print "Total files inserted = %d"%sum


if __name__ == '__main__':
    main()
