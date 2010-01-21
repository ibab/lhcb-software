# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import Escher, LHCbApp, TrackSys

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest database tags for real data
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"
LHCbApp().DDDBtag   = "HEAD"
LHCbApp().CondDBtag = "HEAD"

#from Configurables import ( CondDB, CondDBAccessSvc )
#otCalib = CondDBAccessSvc( 'OTCalib' )
##otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/ConditionsOTCalibration.db/LHCBCOND'
#otCalib.ConnectionString = 'sqlite_file:/afs/cern.ch/user/w/wouter/public/AlignDB/CalibrationOTQuarterT0s_071209.db/LHCBCOND'
#CondDB().addLayer( otCalib )

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
Escher().DatasetName = 'collisions'
Escher().InputType = 'DST'

from Configurables import EventClockSvc
EventClockSvc().InitialTime = 1260497718112960000

prefix = 'PFN:castor:/castor/cern.ch/user/t/truf/data_2009/'
data = [
    '00005727_00000001_00000011_2_nolumi.dst',
    '00005727_00000012_00000026_2_nolumi.dst',
    '00005727_00000027_00000039_2_nolumi.dst',
    '00005727_00000040_00000052_2_nolumi.dst',
    '00005727_00000053_00000057_2_nolumi.dst',
    '00005730_00000001_00000002_2_nolumi.dst',
    '00005731_00000001_00000056_2_nolumi.dst' ]

# copy the files to /pool if it exists
import os
if os.path.isdir( '/pool/spool/wouter/' ) :
    prefix = 'PFN:/pool/spool/wouter/'
    for d in data:
        if not os.path.exists( '/pool/spool/wouter/' + d ):
            os.system('rfcp /castor/cern.ch/user/t/truf/data_2009/' + d + ' /pool/spool/wouter/')
            
EventSelector().Input = []
for d in data:
    name = "DATAFILE='" + prefix + d + "' TYP='POOL_ROOTTREE' OPT='READ'" 
    EventSelector().Input.append( name )

from Configurables import TESCheck
TESCheck().Stop = False
