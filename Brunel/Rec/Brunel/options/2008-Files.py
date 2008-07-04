# Example 2008 data files for Brunel.
# This file must be given as last argument to gaudirun.py, after Brunel<conf>.py

# Syntax is:
#   gaudirun.py Brunel<conf>.py 2008-Files.py
#
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Brunel.Configuration import *

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

#-- Main ('signal') event input
inputType   = Brunel().getProp("inputType").upper()

if inputType == "MDF":
    # Example MDF file. File MUST be described in the FileCatalog, with an FID
    # generated externally, as it is not contained in the MDF file. If the FID is
    # not provided in the Catalog, the output dataset will not contain the
    # information to allow navigation back to the MDF data when reading
    datasetName =  "00001820_00000001"
    # No 2008 MDF file yet, format is as follows:
    # EventSelector().Input = [ "DATA='FID:94ACC0F5-09A3-DC11-8140-003048836626' SVC='LHCb::MDFSelector'" ] # Can use FID or LFN or PFN.

elif inputType == "DST":
    # Example POOL DST (reprocessing)
    datasetName =  "00001820_00000001"
    # No 2008 DST file yet, format is as follows:
    # EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DST/0000/" + datasetName + "_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

elif inputType == "ETC":
    datasetName = "ETC_00002000_00000621"
    FileCatalog().Catalogs.append("xmlcatalog_file:$BRUNELROOT/job/" + datasetName + "_catalog.xml")
    # No 2008 ETC file yet, format is as follows:
    # EventSelector().Input = ["COLLECTION='TagCreator/1' DATAFILE='PFN:castor:/castor/cern.ch/user/p/pkoppenb/Stripping/" + datasetName + ".root'  TYP='POOL_ROOTTREE' SEL='(GlobalOr>=1)'"] # Generated by Joel from DC06 rDSTs
    
else:
    # Example POOL DIGI (default, MC production)
    datasetName =  "11144103-100ev-20080613"
    # B->J/Psi(mumu)Ks events with Boole v15r2, Gauss v31r2, DB tag head-20080603
    EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/Boole/v15r2/" + datasetName + ".digi' TYP='POOL_ROOTTREE' OPT='READ'"]

# Set the property, used to build other file names
Brunel().setProp( "datasetName", datasetName )

#-- Save the monitoring histograms
HistogramPersistencySvc().OutputFile = Brunel().histosName()

#-- Possible output streams. Enabled by setting the corresponding Brunel() property
outputName = Brunel().outputName()
OutputStream("DstWriter").Output = "DATAFILE='PFN:" + outputName + "' TYP='POOL_ROOTTREE' OPT='REC'"

if( inputType == "ETC" ):
    TagCollectionSvc("EvtTupleSvc").Output = [ "EVTTAGS2 DATAFILE='etc-output-test.root' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]
