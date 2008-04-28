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

# Set the property used to build other file names
Boole().setProp( "datasetName", datasetName )

#-- Save the monitoring histograms
HistogramPersistencySvc().OutputFile = Boole().histosName()


#-- Possible output streams. Enabled by setting the corresponding Boole() property
outputName = Boole().outputName()
# writeRawMDF = true:  Simulated raw data, in MDF format, without MC truth. 
OutputStream("RawWriter").Output = "DATAFILE='PFN:" + outputName + ".mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
# writeRawMDF = false: Standard .digi in POOL format. If extendedDigi = true includes also MCHits 
OutputStream("DigiWriter").Output = "DATAFILE='PFN:" + outputName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
# write L0ETC = true: ETC with L0 result
TagCollectionStream( "WR" ).Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + datasetName + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
