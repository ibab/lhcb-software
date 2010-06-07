# Example 2010 data files for Boole.
#
# Syntax is:
#   gaudirun.py Boole<conf>.py 2010-Files.py
#

from Gaudi.Configuration import *
from Configurables import Boole, LHCbApp

#-- File catalogs
FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]

#-- Event input
LHCbApp().DDDBtag   = "head-20100119"
LHCbApp().CondDBtag = "sim-20100321-vc-mu100"

# Minimum bias, Beam3500GeV-VeloClosed-MagDown-Nu1, 2010-Sim02
EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/MC/2010/SIM/00006196/0000/00006196_00000143_1.sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]

# Default output files names are set up using value Boole().DatasetName property
Boole().DatasetName = "00006196_00000143_2"
