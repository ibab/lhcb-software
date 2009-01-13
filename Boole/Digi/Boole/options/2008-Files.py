# Example 2008 data files for Boole.
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py 2008-Files.py
#

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Main ('signal') event input

# Events from Gauss v36r1, DB tag head-200910108 (new muon geometry)
#LHCbApp().DDDBtag   = "head-20090108"
#LHCbApp().CondDBtag = "sim-20090108"
#datasetName = '11144101-100ev-20090111'
#EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2009/v36r1/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]

# Events from Gauss v36r1, DB tag head-200910112
LHCbApp().DDDBtag   = "head-20090112"
LHCbApp().CondDBtag = "sim-20090112"
datasetName = '11144101-100ev-20090112'
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2009/v36r1/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]


#-- Spillover events not used in 2008 configuration


# Default output files names are set up using value Boole().DatasetName property
Boole().DatasetName = datasetName
# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

# writeRawMDF = true:  Simulated raw data, in MDF format, without MC truth. 
#OutputStream("RawWriter").Output = "DATAFILE='PFN:SomeFile.mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"

# writeRawMDF = false: Standard .digi in POOL format. If extendedDigi = true includes also MCHits 
#OutputStream("DigiWriter").Output = "DATAFILE='PFN:SomeFile.digi' TYP='POOL_ROOTTREE' OPT='REC'"

# write L0ETC = true: ETC with L0 result
#TagCollectionStream( "WR" ).Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='SomeETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
