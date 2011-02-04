# Example 2009 Beam gas options for Brunel

# Syntax is:
#   gaudirun.py 2009-BeamGas.py
#
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/moff.py")
importOptions("$APPCONFIGOPTS/Brunel/veloOpen.py")
importOptions("$APPCONFIGOPTS/Brunel/beamGas.py")

from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2009 database tags for real data
Brunel().DataType = "2009"

datasetName = "062509_0000000001"
EventSelector().Input = ["DATAFILE='PFN:root:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/BEAM1/62509/" + datasetName + ".raw'  SVC='LHCb::MDFSelector'"]

# Default output files names are set up using value Brunel().DatasetName property
Brunel().DatasetName = datasetName
