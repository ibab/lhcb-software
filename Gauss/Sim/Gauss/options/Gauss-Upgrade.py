## ############################################################################
## # File for running Gauss with Upgrade database
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade.py <someInputJobConfiguration>.py
## ############################################################################

from Gaudi.Configuration import *
from Configurables import Gauss

## # Here are beam settings as for various nu (i.e. mu and Lumi per bunch with
## # 25 ns spill-over are selected
## nu=2.9 (i.e. mu=2=(nu/0.699, Lumi/bunch=0.31451*(10**30), Lumi=0.88*(10**33)
importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu2.9-25ns.py") 
## nu=4.3 (i.e. mu=3, Lumi=1.32*(10**33)
#importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu4.3-25ns.py")
## nu=5.7 (i.e. mu=4, Lumi=1.77*(10**33)
#importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu5.7.2-25ns.py")
## nu=7.2 (i.e. mu=5, Lumi=2.21*(10**33)
#importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu7.2-25ns.py")

## # The spill-over is switched off for quick tests
Gauss().SpilloverPaths = []

# And the upgrade database is picked up 
# How to select the baseline upgrade configuration or alternative
# configurations will be added in the future
from Configurables import LHCbApp
from Configurables import CondDB

CondDB().Upgrade = True
LHCbApp().DDDBtag   = "dddb-20121001"
LHCbApp().CondDBtag = "sim-20121001-vc-md100"






