########################################################################
#
# Options for CheatedSelection 
#
# @author Marco Musy
# @date 2009-02-23
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer

########################################################################
#
importOptions( "$FLAVOURTAGGINGCHECKEROPTS/CheatedSelection.py" )



########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 100                        # Number of events
#DaVinci().DataType = "2008"                  # Default is "DC06"
DaVinci().Simulation   = True

########################################################################
# HLT
#DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
#DaVinci().Hlt2IgnoreHlt1Decision = True    ## Hlt2 irrespective of Hlt1
DaVinci().HltType = ''                  ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'

########################################################################
# Data file BsDspi:
EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002034/DST/0000/00002034_00000001_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

