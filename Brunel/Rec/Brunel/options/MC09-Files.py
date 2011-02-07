# Example 2009 MC files for Brunel.
# This file must be given as last argument to gaudirun.py, after Brunel<conf>.py

# Syntax is:
#   gaudirun.py Brunel<conf>.py MC09-Files.py
#
from Gaudi.Configuration import *
from Configurables import Brunel, LHCbApp

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

#-- Do not set Database tag for MC09, use the defaults

#-- Event input
inputType   = Brunel().getProp("InputType").upper()

if inputType == "MDF":
    import sys
    print "**************************************"
    print "Do not use MC09-Files.py for real data"
    print "**************************************"
    sys.exit()

elif inputType == "DST":
    import sys
    print "***************************************************"
    print " Please provide a DST input file in MC09-Files.py "
    print "***************************************************"
    sys.exit()

else:
    # Example POOL DIGI (default, MC production)
    # minimum bias events, MC09 default settings, Gauss v17r0 with Boole v18r0

    digiType = Brunel().getProp("DigiType").capitalize()
    if digiType == "Minimal":
        datasetName = '30000000-100ev-20090407-MC09-Minimal'
    else:
        datasetName = '30000000-100ev-20090407-MC09'

    EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Boole/v18r0/" + datasetName + ".digi?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Add patch tags of the DB
#from Configurables import CondDB
#
#CondDB().LocalTags["SIMCOND"] = ["calo-20100613", "calo-20100215", "lumi-20100423", "rich-20100826"]

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
