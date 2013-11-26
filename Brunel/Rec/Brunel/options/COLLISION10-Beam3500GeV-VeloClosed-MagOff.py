# Example 2010 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION10-Beam3500GeV-VeloClosed-MagOff.py
#
from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/moff.py")

from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Brunel

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2010 database tags for real data
Brunel().DataType = "2010"

from PRConfig import TestFileDB
TestFileDB.test_file_db['2010_MagOff_raw_default'].run()
