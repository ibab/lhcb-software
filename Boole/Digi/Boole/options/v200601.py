# File for running Boole on data produced with DC06 geometry 

# Syntax is:
#   gaudirun.py ../options/BooleCommon.py ../options/v200601.py
#
from Gaudi.Configuration import *
from Boole.Configuration import *

##############################################################################
# Steering options
numEvents    = -1    # events to process
skipEvents   = 0     # events to skip
skipSpill    = 0     # spillover events to skip
useSpillover = True  # set to False to disable spillover
generateTAE  = False # set to True to simulate time alignment events
writeRawMDF  = False # set to True to simulate Raw data. Default is POOL .digi
writeL0ETC   = False # set to True to write ETC of L0 selected events
writeL0Only  = False # set to True to write only L0 selected events
extendedDigi = False # set to True to add MCHits to .digi output file
expertHistos = False # set to True to write out expert histos
noWarnings   = False # suppress all messages with MSG::WARNING or below 
datasetName  = '00001820_00000001' # name used to build file names
condDBtag    = "DC06-latest" # conditions database tag
##############################################################################

#-- Choose a geometry
BooleSetDB( condDBtag )

#-- Number of events
BooleSetEvents( numEvents, skipEvents, skipSpill )

#-- Processing options
BooleSetOptions( generateTAE, useSpillover )

#-- Output type
BooleSetOutput( writeRawMDF, extendedDigi, writeL0Only, writeL0ETC, noWarnings )

#-- Save the monitoring histograms, optionally fill and save also expert histos
histosName = BooleHistosName( datasetName, numEvents, expertHistos, generateTAE )
BooleSaveHistos( histosName, expertHistos )

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Main ('signal') event input
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_1.sim' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Spillover events
EventSelector("SpilloverSelector").Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_2.sim' TYP='POOL_ROOTTREE' OPT='READ'"
   ,"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/SIM/0000/" + datasetName + "_3.sim' TYP='POOL_ROOTTREE' OPT='READ'"
    ]

#-- Possible output streams. Enabled by setting the corresponding output type
outputName = BooleOutputName( datasetName, numEvents, generateTAE, writeL0Only, extendedDigi )
# writeRawMDF = true:  Simulated raw data, in MDF format, without MC truth. 
OutputStream("RawWriter").Output = "DATAFILE='file://" + outputName + ".mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
# writeRawMDF = false: Standard .digi in POOL format. If extendedDigi = true includes also MCHits 
OutputStream("DigiWriter").Output = "DATAFILE='PFN:" + outputName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
# write L0ETC = true: ETC with L0 result
TagCollectionStream( "WR" ).Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + datasetName + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
