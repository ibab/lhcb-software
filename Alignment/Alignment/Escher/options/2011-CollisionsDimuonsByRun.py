from GaudiKernel.ProcessJobOptions import importOptions
#importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")

from Configurables import (Escher, LHCbApp, ApplicationMgr, NTupleSvc, ATrackSelector )
from Gaudi.Configuration import FileCatalog, EventSelector


#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

Escher().DataType = "2011"
Escher().Simulation = False
Escher().InputType = "MDF"
#Escher().InputType = "DST"

#-- Use latest 2009 database tags for real data
#LHCbApp().DDDBtag   = "head-20090330"
#LHCbApp().CondDBtag = "head-20090402"

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"
#LHCbApp().DDDBtag   = "HEAD"
#LHCbApp().CondDBtag = "HEAD"
LHCbApp().DDDBtag = 'head-20110823'
LHCbApp().CondDBtag = 'head-20110901'

from Configurables import ( CondDB, CondDBAccessSvc )
cdb = CondDB()
#cdb.PartitionConnectionString["ONLINE"] = "sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/ONLINE-201103.db/ONLINE"
#cdb.Tags["ONLINE"] = "fake"

# maybe it works if we read it as a layer?
myOnline = CondDBAccessSvc( 'MyOnline' )
myOnline.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/ONLINE-2011.db/ONLINE'
CondDB().addLayer( myOnline )
#importOptions("$APPCONFIGOPTS/DisableLFC.py")
cdb.UseOracle = False
cdb.DisableLFC = True

import os
runnr = os.environ['RUNNR']

filenames = [ '/pool/spool/wouter/dimuons_%s.dst' % runnr ]
for f in filenames :
    fullname = "DATAFILE='" + f + "' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( fullname )

print "EvenSelector.Input:", EventSelector().Input

from Configurables import EventClockSvc
#EventClockSvc().InitialTime = 1314000149027776000
EventClockSvc().InitialTime = int(os.environ['INITIALTIME'])

