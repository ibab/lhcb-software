# Example DC06 data files for Boole.
# This file must be given as last argument to gaudirun.py, after Boole<conf>.py

# Syntax is:
#   gaudirun.py Boole<conf>.py DC06-Files.py
#
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Boole.Configuration import *

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Main ('signal') event input
datasetName = "00001820_00000001"
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_1.sim' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Spillover events
EventSelector("SpilloverSelector").Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_2.sim' TYP='POOL_ROOTTREE' OPT='READ'"
   ,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_3.sim' TYP='POOL_ROOTTREE' OPT='READ'"
    ]

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
