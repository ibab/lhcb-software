#
#   Stripping selections job (DST output)
#
#   @author J. Palacios/A. Poluektov
#   @date 2009-11-05
#

from Gaudi.Configuration import *
from Configurables import PrintDecayTree, DaVinci
from Configurables import CountParticles

##############################################################################
#
# Ks
#
kalgs = []
lalgs = []
for i in ['','_noPV','_noPV_ws','_ws' ]:
    lalgs.append('StrippingLambda0'+i)
    kalgs.append('StrippingK0S'+i)
kalgs.append('StrippingKSAllGeoLineCombine')
lalgs.append('StrippingLambdaAllGeoLineCombine')
algs = kalgs + lalgs

head = '/Event/Strip/Phys/'
locs = []
for i in algs :
    locs.append( head+i )

CC = CountParticles(InputLocations = locs )
##############################################################################
#
# DaVinci
#
DaVinci().DataType = "2009"                   # Default is "MC09"
from Configurables import StoreExplorerAlg
DaVinci().UserAlgorithms = [ CC ]  # , PT
DaVinci().EvtMax = 10000
DaVinci().PrintFreq = 100
##############################################################################
#
# Real Data
#
EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/data/2009/DST/00005803/0000/00005803_00000001_1.V0.dst?svcClass=lhcbmdst&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'" ]


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
