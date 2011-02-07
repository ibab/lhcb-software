# Example DC06 data files for Brunel.
# This file must be given as last argument to gaudirun.py, after Brunel<conf>.py

# Syntax is:
#   gaudirun.py Brunel<conf>.py DC06-Files.py
#
from Gaudi.Configuration import *
from Configurables import Brunel, LHCbApp

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

#-- Main ('signal') event input
inputType   = Brunel().getProp("InputType").upper()

#-- DC06 database tags are backwards compatible by construction, use default
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

if inputType == "MDF":
    # Example MDF file. File MUST be described in the FileCatalog, with an FID
    # generated externally, as it is not contained in the MDF file. If the FID is
    # not provided in the Catalog, the output dataset will not contain the
    # information to allow navigation back to the MDF data when reading
    datasetName =  "00001820_00000001"
    # B->J/Psi(mumu)Ks events with Boole v12r4, Gauss v25r2, XmlDDDB v30r8
    EventSelector().Input = [ "DATA='FID:94ACC0F5-09A3-DC11-8140-003048836626' SVC='LHCb::MDFSelector'" ] # Can use FID or LFN or PFN.

elif inputType == "DST":
    # Example POOL DST (reprocessing)
    datasetName =  "00001820_00000001"
    # B->J/Psi(mumu)Ks events
    EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Brunel/v35r2/" + datasetName + ".dst?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]

else:
    # Example POOL DIGI
    datasetName =  "00001820_00000001"
    # B->J/Psi(mumu)Ks events with Boole v17r2
    EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Boole/v17r2/" + datasetName + ".digi?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Example field off and velo open files
if "fieldOff" in Brunel().getProp("SpecialData") :
    if "veloOpen" in Brunel().getProp("SpecialData") :
        # 450 GeV on 450 GeV pp collisions, single interactions, no beam crossing angle, beam sigma(X) = sigma(Y) = 280 microns, magnetic field OFF, Velo Open
        datasetName = "B_OFF.VeloOpen"
        EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/lhcb/background/Pilot_2007/30000000.5k." + datasetName + ".digi?svcClass=default' TYP='POOL_ROOT' OPT='READ'"]
    else:
        # 450 GeV on 450 GeV pp collisions, single interactions, no beam crossing angle, beam sigma(X) = sigma(Y) = 280 microns, magnetic field OFF
        datasetName = "B_OFF.VeloClose"
        EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/lhcb/background/Pilot_2007/30000000.5k." + datasetName + ".digi?svcClass=default' TYP='POOL_ROOT' OPT='READ'"]

# Default output files names are set up using value Brunel().DatasetName property
Brunel().DatasetName = datasetName

# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

#-- Dst or rDst file
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.dst' TYP='POOL_ROOTTREE' OPT='REC'"
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.rdst' TYP='POOL_ROOTTREE' OPT='REC'"

#-- ETC output in case of ETC input
#TagCollectionSvc("EvtTupleSvc").Output = [ "EVTTAGS2 DATAFILE='some-etc.root' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]
