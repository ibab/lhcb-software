#!/usr/bin/env python
from PyCool import cool,pool
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
        pnew = re.sub("/[^/.]*/\.\./","/",p) # remove relative pathes. FIX-ME it is not perfect
        while p != pnew: # iterate
            p = pnew
            pnew = re.sub("/[^/.]*/\.\./","/",p) # remove relative pathes
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

connectString = "oracle://intr1-v.cern.ch/INTR;schema=lhcb_cool;user=lhcb_cool;dbname=DDDB"
#connectString = "sqlite://none;schema=XmlDDDB_v26r2.sqlite;user=none;password=none;dbname=DDDB"
#connectString = "mysql://pclhcb91.cern.ch;schema=MARCOCLE;user=marcocle;dbname=DDDB"
#connectString = "oracle://oradev10.cern.ch:10520/D10;schema=lhcb_lcg_cool_dev;user=lhcb_lcg_cool_dev;dbname=DDDB"

DDDB_top = os.path.join(os.environ['XMLDDDBROOT'],'DDDB')

folderspec = cool.ExtendedAttributeListSpecification()
#folderspec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_255)
#folderspec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_4K)
#folderspec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_64K)
folderspec.push_back("data","string",cool.PredefinedStorageHints.STRING_MAXSIZE_16M)
folderdesc = "<storage_type=7>"

payload = pool.AttributeList(folderspec.attributeListSpecification())
payload["data"] = "XML stuff"

dbs = cool.DatabaseSvcFactory.databaseService()

dbs.dropDatabase(connectString)

db = dbs.createDatabase(connectString)

excludes = [ 'clhep2dtd.pl' ]

# count files (just to know)
sum = 0
for root, dirs, files in os.walk(DDDB_top):
    if 'CVS' in root: # skip CVS directories
        continue
    for x in excludes:
        if x in files:
            files.remove(x)
    sum += len(files)

num_len = len(str(sum))
# populate the database
n = 0
for root, dirs, files in os.walk(DDDB_top):
    # remove base path
    dbroot = root.replace(DDDB_top,"")
    if 'CVS' in root: # skip CVS directories
        continue

    if dbroot != "": # it means it is not "/"
        db.createFolderSet(dbroot)
    else:
        dbroot = '/'
    
    print dbroot
    for x in excludes:
        if x in files:
            files.remove(x)
    for f in files:
        n+=1
        print ("%" + str(num_len) + "d %" + str(num_len) + "d  %s")%(n,sum-n,f)
        folder = db.createFolderExtended(os.path.join(dbroot,f),
                                         folderspec,folderdesc,
                                         cool.FolderVersioning.MULTI_VERSION)
        xml_data = open(os.path.join(root,f)).read()
        fixed_data = fix_system_ids(xml_data,"conddb:"+dbroot)
        fixed_data = fix_env_vars(fixed_data)
        if fixed_data.find('Velo/v200507/DTD/geometry.dtd') >= 0 :
            print fixed_data
        payload["data"] = fixed_data
        folder.storeObject(cool.ValidityKeyMin,cool.ValidityKeyMax,payload,0)

print "Total %d files"%sum
