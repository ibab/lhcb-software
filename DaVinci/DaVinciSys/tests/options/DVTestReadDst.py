########################################################################
#
# $Id: DVTestReadDst.py,v 1.10 2009-11-11 18:44:50 pkoppenb Exp $
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
# Print the J/psis
#
from Configurables import PrintDecayTree, PrintHeader, FilterDesktop
DaVinci().UserAlgorithms = [ PrintHeader() ]
MassRanges = [ [ 2600, 3200 ], [ 3050, 3150 ],  [ 3000, 3500 ] ]
for i in MassRanges :
    ln = str(i[0])
    hn = str(i[1])
    name = ln+"_"+hn

    pJpsi = PrintDecayTree('PrintJpsi_'+name
                         , InputLocations = [ "/Event/Sel/Phys/Jpsi_"+name  ])
    DaVinci().UserAlgorithms += [ pJpsi ]
    f = FilterDesktop('Filter_'+name,
                      InputLocations = [ "/Event/Sel/Phys/Jpsi_"+name  ],
                      Code = "(MIPCHI2DV(PRIMARY) > 2)")
    DaVinci().UserAlgorithms += [ f ]
    
##############################################################################
#
#
##############################################################################

DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1 
DaVinci().DataType = "MC09"
DaVinci().Input = [ "DATAFILE='PFN:$DAVINCISYSROOT/tests/qmtest/Sel.SeqJpsi_3050_3150.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:$DAVINCISYSROOT/tests/qmtest/Sel.SeqJpsi_2600_3200.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:$DAVINCISYSROOT/tests/qmtest/Sel.SeqJpsi_3000_3500.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
                    ]
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
