########################################################################
#
# $Id: DVTestReadDst.py,v 1.5 2009-07-09 09:38:45 pkoppenb Exp $
#
# Options for a typical DaVinci job creating DSTs
#
# @author Patrick Koppenburg
# @date 2009-02-04
#
########################################################################
from Gaudi.Configuration import *
from Configurables import DaVinci
##############################################################################
#
# Print the J/psi
#
from Configurables import PrintDecayTree, PrintHeader
pJpsi = PrintDecayTree('PrintJpsi')
pJpsi.InputLocations = [ "Jpsi_3050_3150", "Jpsi_2600_3200", "Jpsi_3000_3500"  ]
# pJpsi.OutputLevel = 1
#
ph = PrintHeader()
##############################################################################
#
#
##############################################################################

DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1 
DaVinci().DataType = "MC09" # Default is "DC06"
DaVinci().Input = [ "DATAFILE='PFN:$DAVINCISYSROOT/tests/qmtest/Jpsi_3050_3150.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:$DAVINCISYSROOT/tests/qmtest/Jpsi_2600_3200.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:$DAVINCISYSROOT/tests/qmtest/Jpsi_3000_3500.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
                    ]
DaVinci().UserAlgorithms = [  ph, pJpsi ]
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
