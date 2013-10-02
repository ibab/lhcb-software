# Example 2011 collisions options for Brunel

# Syntax is:
#   gaudirun.py COLLISION12-tests.py
#
from Gaudi.Configuration import FileCatalog, EventSelector
from Configurables import Escher

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

#-- Use latest 2011 database tags for real data
Escher().DataType = "2012"
Escher().InputType  = "MDF"
Escher().DDDBtag = 'dddb-20130503-1'
Escher().CondDBtag = 'cond-20130710'

# Events from run 123928 from fill 2880, on 26th July 2012
from GaudiConf import IOHelper
IOHelper('MDF').inputFiles([
      'mdf:root://eoslhcb.cern.ch//eos/lhcb/freezer/lhcb/data/2012/RAW/EXPRESS/LHCb/COLLISION12/123928/123928_0000000108.raw'
    ], clear=True)
