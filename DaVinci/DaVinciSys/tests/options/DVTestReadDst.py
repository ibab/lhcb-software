########################################################################
#
# $Id: DVTestReadDst.py,v 1.3 2009-04-29 13:27:53 pkoppenb Exp $
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
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Input = [ "DATAFILE='PFN:Jpsi_3050_3150.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:Jpsi_2600_3200.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:Jpsi_3000_3500.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
                    ]
DaVinci().UserAlgorithms = [  ph, pJpsi ]
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
