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

CC = CountParticles(Inputs = locs )
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
"   DATAFILE='castor:/castor/cern.ch/user/r/rlambert/testfiles/2009_00005803_00000001_1.V0.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
