from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")

from Configurables import (Escher, LHCbApp, ApplicationMgr, NTupleSvc, ATrackSelector )
from Gaudi.Configuration import FileCatalog, EventSelector


#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

Escher().DataType = "2010"
Escher().Simulation = False
Escher().SpecialData += [ "earlyData"]
Escher().InputType = "MDF"
#Escher().InputType = "DST"

#-- Use latest 2009 database tags for real data
#LHCbApp().DDDBtag   = "head-20090330"
#LHCbApp().CondDBtag = "head-20090402"

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"
LHCbApp().DDDBtag   = "HEAD"
LHCbApp().CondDBtag = "HEAD"

from Configurables import EventClockSvc
EventClockSvc().InitialTime = 1270079584012864000
from Configurables import ( CondDB, CondDBAccessSvc )
cdb = CondDB()
#cdb.PartitionConnectionString["ONLINE"] = "sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/ONLINE-201004.db/ONLINE"
#cdb.Tags["ONLINE"] = ""

# maybe it works if we read it as a layer?
myOnline = CondDBAccessSvc( 'MyOnline' )
myOnline.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/ONLINE-201004.db/ONLINE'
CondDB().addLayer( myOnline )

otCalib = CondDBAccessSvc( 'OTCalib' )
#otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/a/akozlins/public/OT/LHCBCOND/ModuleT0s_2.7ns_3.35ns_180310.db/LHCBCOND'
otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/a/akozlins/public/OT/LHCBCOND/ModuleT0s_69648_140410.db/LHCBCOND'
CondDB().addLayer( otCalib )

ttPitchFix = CondDBAccessSvc( 'TTPitchFix' )
ttPitchFix.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/TTPitchFix.db/DDDB'
CondDB().addLayer( ttPitchFix )

import os
path = '/castor/cern.ch/grid/lhcb/data/2010/RAW/FULL/LHCb/COLLISION10/'
runnrs = ['69355']
data = []
for runnr in runnrs:
    put, get = os.popen4("rfdir " + path  + runnr)
    for line in get.readlines():
        splittedline = line.split(' ')
        filestring = splittedline[len(splittedline)-1].strip(' ')
        filestring = filestring.replace('\n','')
        print 'adding file: ', runnr + "/" + filestring
        data.append( (path + runnr , filestring) )

# let's take only the first one
data = [ data[0] ]

print "found data files: ", data

# now stage the files locally. they will not like this:-)
EventSelector().Input = []
import os
targetdir = '/tmp/wouter/'

if os.path.isdir( '/pool/spool/' ) :
    if not os.path.exists( '/pool/spool/wouter' ):
        os.system('mkdir /pool/spool/wouter')
    targetdir = '/pool/spool/wouter/'

for d in data:
    print 'd:', d
    if not os.path.exists( targetdir + d[1] ):
        os.system('rfcp ' + d[0] + "/" + d[1] + ' ' + targetdir)
    EventSelector().Input.append( "DATAFILE='" + targetdir +  d[1] + "' " +"SVC='LHCb::MDFSelector'")

print "EvenSelector.Input:", EventSelector().Input

#from Configurables import CondDB
#CondDB(UseOracle = True)
