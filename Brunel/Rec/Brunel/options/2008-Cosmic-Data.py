# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Brunel.Configuration import *

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
datasetName = "035537_0000088110"
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/COSMICS/35537/" + datasetName + ".raw'  SVC='LHCb::MDFSelector'"]

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
