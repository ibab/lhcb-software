# Example MC10 data files for Boole.
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py MC10-Files.py
#

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp

# L0 TCK for MC10
from Configurables import L0Conf
L0Conf().TCK = '0x002A'

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Event input
LHCbApp().DDDBtag   = "head-20101206"
LHCbApp().CondDBtag = "sim-20101210-vc-md100"

# Minimum bias events from Gauss v39r4, default MC10 settings
datasetName = '30000000-100ev-20110902-MC10'

EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/g/gcorti/Gauss/2010/v39r4/" + datasetName + ".sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]

# Default output files names are set up using value Boole().DatasetName property
Boole().DatasetName = datasetName
# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

# writeRawMDF = true:  Simulated raw data, in MDF format, without MC truth. 
#OutputStream("RawWriter").Output = "DATAFILE='PFN:SomeFile.mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"

# writeRawMDF = false: Standard .digi in POOL format. If extendedDigi = true includes also MCHits 
#OutputStream("DigiWriter").Output = "DATAFILE='PFN:SomeFile.digi' TYP='POOL_ROOTTREE' OPT='REC'"
