########################################################################
#
# $Id: DVTestReadDst.py,v 1.11 2009/11/30 10:57:40 jpalac Exp $
#
# Options for a typical DaVinci job reading DSTs with saved candidates.
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
MassRanges = [ [ 3000, 3087 ], [ 3088, 3100 ],  [ 3100, 4000 ] ]
for i in MassRanges :
    ln = str(i[0])
    hn = str(i[1])
    name = ln+"_"+hn
    location = "/Event/SeqJpsi_" + name + "/Phys/SelJpsi_" + name
    pJpsi = PrintDecayTree('PrintJpsi_'+name,
                           Inputs = [ location  ])
    DaVinci().UserAlgorithms += [ pJpsi ]
    f = FilterDesktop('Filter_'+name,
                      Inputs = [ location  ],
                      Code = "(MIPCHI2DV(PRIMARY) > 2)")
    DaVinci().UserAlgorithms += [ f ]
    
##############################################################################
#
#
##############################################################################

DaVinci().EvtMax = -1
DaVinci().PrintFreq = 1 
DaVinci().DataType = "2010"
DaVinci().Simulation = True
DaVinci().Input = [ "DATAFILE='PFN:$DAVINCITESTSROOT/tests/qmtest/Sel.SeqJpsi_3088_3100.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:$DAVINCITESTSROOT/tests/qmtest/Sel.SeqJpsi_3100_4000.dst' TYP='POOL_ROOTTREE' OPT='READ'"
                 ,  "DATAFILE='PFN:$DAVINCITESTSROOT/tests/qmtest/Sel.SeqJpsi_3000_3087.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
                    ]
########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
