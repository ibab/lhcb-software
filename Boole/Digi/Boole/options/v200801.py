# Example file for running Boole on data produced with Gaudi v31r* or greater 

# Syntax is:
#   gaudirun.py ../options/BooleCommon.py ../options/v200801.py
#
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Boole.Configuration import *

##############################################################################
# Available steering options with defaults.
# Edit one or more of these lines to change the defaults
# Boole().EvtMax          = -1    # Number of events to process
# Boole().skipEvents      = 0     # Number of events to skip before first event
# Boole().skipSpill       = 0     # Number of spillover events to skip
# Boole().useSpillover    = True  # set to False to disable spillover
# Boole().generateTAE     = False # set to True to simulate time alignment events
# Boole().writeRawMDF     = False # set to True to write simulated Raw data
# Boole().writeDigi       = True  # set to False to not write POOL .digi
# Boole().writeL0ETC      = False # set to True to write ETC of L0 selected events
# Boole().writeL0Only     = False # set to True to write only L0 selected events
# Boole().extendedDigi    = False # set to True to add MCHits to .digi output file
# Boole().expertHistos    = False # set to True to write out expert histos
# Boole().noWarnings      = False # set to True to suppress all MSG::WARNING or below 
# Boole().datasetName     = '00001820_00000001' # string used to build file names
# Boole().DDDBtag         = "DC06-latest" # geometry database tag
# Boole().condDBtag       = "DC06-latest" # conditions database tag
##############################################################################

Boole().datasetName = '11144103-500ev-20080228' # Events from Gauss v30r1
Boole().condDBtag   = "head-20080130"

##############################################################################
# IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
# The following line must always be *after* all Boole steering options
##############################################################################
Boole().applyConf()

##############################################################################
# Remaining options define I/O files. Functions to build the file names provide
# suggested default names, but don't have to be used!!
##############################################################################

#-- Save the monitoring histograms
histosName = Boole().histosName()
HistogramPersistencySvc().OutputFile = histosName

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Main ('signal') event input
datasetName = Boole().getProp("datasetName")
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2008/v31r0/" + datasetName + ".sim' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Spillover events
EventSelector("SpilloverSelector").Input = [
    "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcorti/Gauss/2008/v31r0/30000000-1000ev-20080223.sim' TYP='POOL_ROOTTREE' OPT='READ'"
    ]

#-- Possible output streams. Enabled by setting the corresponding Boole() property
outputName = Boole().outputName()
# writeRawMDF = true:  Simulated raw data, in MDF format, without MC truth. 
OutputStream("RawWriter").Output = "DATAFILE='PFN:" + outputName + ".mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
# writeDigi = true: Standard .digi in POOL format. If extendedDigi = true includes also MCHits 
OutputStream("DigiWriter").Output = "DATAFILE='PFN:" + outputName + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
# write L0ETC = true: ETC with L0 result
TagCollectionStream( "WR" ).Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + datasetName + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
