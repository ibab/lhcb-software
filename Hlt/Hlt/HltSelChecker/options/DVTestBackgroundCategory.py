## @file 
 #  Test file for HLT background category
 #
 #  @author P. Koppenburg
 #  @date 2007-08-08
 #
##--------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import HltBackgroundCategory
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 10000
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
# DaVinci().Hlt2IgnoreHlt1Decision = True        # Ignore Hlt1 in 2
# DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "2008"
DaVinci().Simulation = True 
DaVinci().MoniSequence += [ HltBackgroundCategory() ]
