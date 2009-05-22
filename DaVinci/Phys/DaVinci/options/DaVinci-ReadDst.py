########################################################################
#
# $Id: DaVinci-ReadDst.py,v 1.6 2009-05-22 15:30:46 pkoppenb Exp $
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
pJpsi.InputLocations = [ "MyJpsi"  ]
#
ph = PrintHeader()
##############################################################################
#
#
##############################################################################
importOptions("$STRIPPINGSELECTIONSROOT/options/Stripping.py")

DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1 
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Input = [ "DATAFILE='PFN:Jpsi.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
DaVinci().UserAlgorithms = [  ph, pJpsi ]
