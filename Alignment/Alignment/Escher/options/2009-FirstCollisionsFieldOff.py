# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import Escher, LHCbApp, TrackSys, EventClockSvc

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest database tags for real data
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"
LHCbApp().DDDBtag   = "HEAD"
LHCbApp().CondDBtag = "HEAD"

#-- Set a reasonable time for the first event
EventClockSvc().InitialTime = 1260350949785664000

from Configurables import ( CondDB, CondDBAccessSvc )
otCalib = CondDBAccessSvc( 'OTCalib' )
#otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/ConditionsOTCalibration.db/LHCBCOND'
#otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/CalibrationOTQuarterT0s_071209.db/LHCBCOND'
otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/a/akozlins/public/OT/LHCBCOND/Collision09_OT_ModuleT0s_220110_sigma4ns.db/LHCBCOND'
CondDB().addLayer( otCalib )

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
Escher().DatasetName = 'collisions'
Escher().InputType = 'MDF'

#EventSelector().Input.append("DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/BEAM1/62558/062558_0000000001.raw'  SVC='LHCb::MDFSelector'")
#import os
#runnr = os.environ['COSMICSRUNNR']

runnrs = ['63596']

path = '/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COLLISION09/'
data = []
for runnr in runnrs:
    put, get = os.popen4("rfdir " + path  + runnr)
    for line in get.readlines():
        splittedline = line.split(' ')
        filestring = splittedline[len(splittedline)-1].strip(' ')
        filestring = filestring.replace('\n','')
        print 'adding file: ', runnr + "/" + filestring
        data.append('PFN:castor:' + path + runnr + "/" + filestring)

data = ['/pool/spool/wouter/063596_0000000001.raw']

EventSelector().Input = []
for d in data:
    name = "DATAFILE='" + d + "' SVC='LHCb::MDFSelector'"
    EventSelector().Input.append( name )

#from Configurables import CondDB
#CondDB(UseOracle = True)


#Escher().DatasetName = ''
#EventSelector().Input.append("DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/BEAM1/63213/062558_0000000001.raw'  SVC='LHCb::MDFSelector'")

