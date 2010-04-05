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
otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/a/akozlins/public/OT/LHCBCOND/ModuleT0s_2.7ns_3.35ns_180310.db/LHCBCOND'
CondDB().addLayer( otCalib )

ttPitchFix = CondDBAccessSvc( 'TTPitchFix' )
ttPitchFix.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/TTPitchFix.db/DDDB'
CondDB().addLayer( ttPitchFix )

EventSelector().Input = [
    "DATAFILE='/pool/spool/wouter/069355_0000000001.raw' SVC='LHCb::MDFSelector'"
    ]

#from Configurables import CondDB
#CondDB(UseOracle = True)
