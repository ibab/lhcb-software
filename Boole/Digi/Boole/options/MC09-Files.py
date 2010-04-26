# Example MC09 data files for Boole.
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py MC09-Files.py
#

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp

# L0 TCK for MC09
from Configurables import L0Conf
L0Conf().TCK = '0xDC09'

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Event input
LHCbApp().DDDBtag   = "head-20090508"
LHCbApp().CondDBtag = "sim-20090508-vc-mu100"

# Signal events from Gauss v37r0
#datasetName = '11144103-100ev-20090402'

# Minimum bias events from Gauss v37r0, High Luminosity
#datasetName = '30000000-100ev-20090405-HL'

# Minimum bias events from Gauss v37r0, Normal Luminosity
#datasetName = '30000000-100ev-20090407-NL'

# Minimum bias events from Gauss v37r0, default MC09 settings
datasetName = '30000000-100ev-20090407-MC09'

EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2009/v37r0/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]

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
