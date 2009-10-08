# Example input files for Boole containing spillover
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py Spillover-Files.py
#

from Gaudi.Configuration import EventSelector
from Configurables import Boole, LHCbApp

#-- File catalogs
#FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Event input
LHCbApp().DDDBtag   = "head-20090508"
LHCbApp().CondDBtag = "sim-20090508-vc-mu100"

# Signal events from Gauss v37r3
datasetName = '11144103-100ev-25ns-20090715'

EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2009/v37r3/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]

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
