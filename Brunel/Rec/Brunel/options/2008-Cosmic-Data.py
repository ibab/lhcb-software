# Example 2008 MC files for Brunel. For real data use 2008-TED-Data.py
# This file must be given as last argument to gaudirun.py, after Brunel<conf>.py

# Syntax is:
#   gaudirun.py Brunel<conf>.py 2008-Files.py
#
from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Brunel.Configuration import *

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

# Latest cosmic run, with CALO, OT and (!!) RICH2
datasetName = "035537_0000088110"
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/COSMICS/35537/" + datasetName + ".raw'  SVC='LHCb::MDFSelector'"]
#EventSelector().Input = ["DATAFILE='PFN:/usera/jonesc/NFS/data/LHCb/COSMICS/35537/" + datasetName + ".raw'  SVC='LHCb::MDFSelector'"]

# Set the property, used to build other file names
Brunel().setProp( "datasetName", datasetName )

#-- Save the monitoring histograms
HistogramPersistencySvc().OutputFile = Brunel().histosName()

#-- Possible output streams. Enabled by setting the corresponding Brunel() property
outputName = Brunel().outputName()
OutputStream("DstWriter").Output = "DATAFILE='PFN:/usera/jonesc/NFS/data/2008-Sim/" + outputName + "' TYP='POOL_ROOTTREE' OPT='REC'"
