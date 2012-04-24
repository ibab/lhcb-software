########################################################################
#
# $Id: DaVinci-ReadDst.py,v 1.9 2010-02-19 17:03:59 pkoppenb Exp $
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
pJpsi.Inputs = [ "MyJpsi"  ]
pMC = PrintMCTree()
pMC.ParticleNames = [ 'J/psi(1S)' ]
#
ph = PrintHeader()
#
from Configurables import DecayTreeTuple
tuple = DecayTreeTuple("Tuple")
tuple.Inputs = [ "MyJpsi" ]
tuple.ToolList +=  [ "TupleToolMCBackgroundInfo", "TupleToolMCTruth", "TupleToolKinematic" ]
tuple.Decay = "J/psi(1S) -> ^mu+ ^mu-"

##############################################################################
#
#
##############################################################################

DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1 
DaVinci().DataType = "2009" 
DaVinci().Input = [ "DATAFILE='PFN:Jpsi.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
DaVinci().UserAlgorithms = [  ph, pJpsi, pMC, tuple ]
DaVinci().TupleFile = "Tuple.root"
