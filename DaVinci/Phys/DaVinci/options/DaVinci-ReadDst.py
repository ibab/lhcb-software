########################################################################
#
# $Id: DaVinci-ReadDst.py,v 1.8 2009-06-03 15:24:49 pkoppenb Exp $
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
from Configurables import PrintDecayTree, PrintHeader, PrintMCTree
pJpsi = PrintDecayTree('PrintJpsi')
pJpsi.InputLocations = [ "MyJpsi"  ]
pMC = PrintMCTree()
pMC.ParticleNames = [ 'J/psi(1S)' ]
#
ph = PrintHeader()
#
from Configurables import DecayTreeTuple
tuple = DecayTreeTuple("Tuple")
tuple.InputLocations = [ "MyJpsi" ]
tuple.ToolList +=  [ "TupleToolMCBackgroundInfo", "TupleToolMCTruth", "TupleToolKinematic" ]
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"

##############################################################################
#
#
##############################################################################

DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1 
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Input = [ "DATAFILE='PFN:Jpsi.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
DaVinci().UserAlgorithms = [  ph, pJpsi, pMC, tuple ]
DaVinci().TupleFile = "Tuple.root"
