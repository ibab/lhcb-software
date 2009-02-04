########################################################################
#
# $Id: DaVinci-ReadDst.py,v 1.1 2009-02-04 18:48:01 pkoppenb Exp $
#
# Options for a typical DaVinci job creating DSTs
#
# @author Patrick Koppenburg
# @date 2009-02-04
#
########################################################################
from Configurables import DaVinci
##############################################################################
from Configurables import CheckPV
cpv = CheckPV()
cpv.Print = True
#
# Get a J/psi
#
from Configurables import PrintTree, PhysDesktop
pJpsi = PrintTree('PrintJpsi')
pJpsi.addTool(PhysDesktop())
pJpsi.PhysDesktop.InputLocations = [ 'MyJpsi' ]

##############################################################################
#
#
##############################################################################
DaVinci().EvtMax = -1
DaVinci().DataType = "2008" # Default is "DC06"
DaVinci().Input = [ "DATAFILE='PFN:PV1.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                   ,  "DATAFILE='PFN:PV2.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                   ,  "DATAFILE='PFN:PV3.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
#                  ,  "DATAFILE='PFN:PV4.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
                   ,  "DATAFILE='PFN:PV5.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                    ]
DaVinci().UserAlgorithms = [ cpv, pJpsi ]
##############################################################################
# DaVinci().DstFiles = { "Test.dst" : dstSeq }
