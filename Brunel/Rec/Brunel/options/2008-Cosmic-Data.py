# Example 2008 Cosmics files for Brunel.

# Syntax is:
#   gaudirun.py Brunel-Cosmics.py 2008-Cosmic-Data.py
#
from Gaudi.Configuration import *
from Configurables import Brunel, LHCbApp

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2008 database tags for real data
Brunel().DataType = "2008"

# Latest cosmic run, with CALO, OT and (!!) RICH2 (35569 events)
datasetName = "035537_0000088110"
EventSelector().Input = ["DATAFILE='PFN:mdf:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/COSMICS/35537/" + datasetName + ".raw?svcClass=lhcbtape'  SVC='LHCb::MDFSelector'"]

# Default output files names are set up using value Brunel().DatasetName property
Brunel().DatasetName = datasetName

# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

#-- Dst or rDst file
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.dst' TYP='POOL_ROOTTREE' OPT='REC'"
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.rdst' TYP='POOL_ROOTTREE' OPT='REC'"
