# Example 2008 data files for Boole.
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py 2008-Files.py
#

from Gaudi.Configuration import *
from Configurables import Boole

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Main ('signal') event input
# Events from Gauss v35r1, DB tag head-20081002
datasetName = '11144101-100ev-20081006'
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2008/v35r1/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]

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
