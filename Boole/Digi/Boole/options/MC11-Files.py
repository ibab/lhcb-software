# Example MC11 data files for Boole.
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py MC11-Files.py
#

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp

# L0 TCK for MC11
from Configurables import L0Conf
L0Conf().TCK = '0x0037'

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Event input
LHCbApp().DDDBtag   = "head-20110914"
LHCbApp().CondDBtag = "sim-20110723-vc-md100"

# Generic b events with spillover from Gauss v40r4, settings as in $APPCONFIGOPTS/Gauss/beam35000GeV-md100-MC11-nu2-50ns.py
datasetName = 'Gauss-10000000-100ev-20111014'

EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2011/v40r4/" + datasetName + ".sim'"]

# Default output files names are set up using value Boole().DatasetName property
Boole().DatasetName = datasetName
# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

# writeRawMDF = true:  Simulated raw data, in MDF format, without MC truth. 
#OutputStream("RawWriter").Output = "DATAFILE='PFN:SomeFile.mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"

# writeRawMDF = false: Standard .digi in POOL format. If extendedDigi = true includes also MCHits 
#OutputStream("DigiWriter").Output = "DATAFILE='PFN:SomeFile.digi' TYP='POOL_ROOTTREE' OPT='REC'"
